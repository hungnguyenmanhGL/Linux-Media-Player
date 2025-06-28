#include "AudioProcessor.h"

void AudioProcessor::Cleanup() {
    if (swr_ctx) {
        swr_free(&swr_ctx);
        swr_ctx = nullptr;
    }
    if (codec_ctx) {
        avcodec_free_context(&codec_ctx);
        codec_ctx = nullptr; 
    }
    if (format_ctx) {
        avformat_close_input(&format_ctx);
        format_ctx = nullptr; 
    }
    audio_stream_index = -1;
}

bool AudioProcessor::ProcessAudioFile(const string& path, AudioData& audio_data) {
    Cleanup();

    if (avformat_open_input(&format_ctx, path.c_str(), nullptr, nullptr) < 0) {
        cerr << "[AUDIO_PROCESSOR] Could not open file " << path << endl;
        Cleanup();
        return false;
    }

    // Retrieve stream information
    if (avformat_find_stream_info(format_ctx, nullptr) < 0) {
        cerr << "[AUDIO_PROCESSOR] Could not find stream information" << endl;
        Cleanup();
        return false;
    }

    // Find the first audio stream
    audio_stream_index = -1;
    for (unsigned int i = 0; i < format_ctx->nb_streams; i++) {
        if (format_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            audio_stream_index = i;
            break;
        }
    }

    if (audio_stream_index == -1) {
        cerr << "[AUDIO_PROCESSOR] No audio stream found" << endl;
        Cleanup();
        return false;
    }

    // Get codec parameters
    AVCodecParameters* codec_params = format_ctx->streams[audio_stream_index]->codecpar;

    // Find decoder
    const AVCodec* codec = avcodec_find_decoder(codec_params->codec_id);
    if (!codec) {
        cerr << "[AUDIO_PROCESSOR] Unsupported codec" << endl;
        Cleanup();
        return false;
    }

    // Allocate codec context
    codec_ctx = avcodec_alloc_context3(codec);
    if (!codec_ctx) {
        cerr << "[AUDIO_PROCESSOR] Could not allocate codec context" << endl;
        Cleanup();
        return false;
    }

    // Copy codec parameters to context
    if (avcodec_parameters_to_context(codec_ctx, codec_params) < 0) {
        cerr << "[AUDIO_PROCESSOR] Could not copy codec parameters" << endl;
        Cleanup();
        return false;
    }

    // Open codec
    if (avcodec_open2(codec_ctx, codec, nullptr) < 0) {
        cerr << "[AUDIO_PROCESSOR] Could not open codec" << endl;
        Cleanup();
        return false;
    }

    // Setup SDL audio format (16-bit signed, stereo)
    audio_data.sample_rate = 44100;  // Standard sample rate for SDL
    audio_data.channels = 2;         // Stereo
    audio_data.format = AUDIO_S16SYS; // 16-bit signed

    // Setup resampler
    swr_ctx = swr_alloc();
    if (!swr_ctx) {
        cerr << "[AUDIO_PROCESSOR] Could not allocate resampler" << endl;
        Cleanup();
        return false;
    }

    AVChannelLayout in_ch_layout;
    if (codec_ctx->ch_layout.nb_channels > 0) {
        av_channel_layout_copy(&in_ch_layout, &codec_ctx->ch_layout);
    }
    else {
        av_channel_layout_default(&in_ch_layout, codec_ctx->ch_layout.nb_channels);
    }

    av_opt_set_chlayout(swr_ctx, "in_chlayout", &in_ch_layout, 0);
    av_opt_set_int(swr_ctx, "in_sample_rate", codec_ctx->sample_rate, 0);
    av_opt_set_sample_fmt(swr_ctx, "in_sample_fmt", codec_ctx->sample_fmt, 0);

    AVChannelLayout out_ch_layout = AV_CHANNEL_LAYOUT_STEREO;
    av_opt_set_chlayout(swr_ctx, "out_chlayout", &out_ch_layout, 0);
    av_opt_set_int(swr_ctx, "out_sample_rate", audio_data.sample_rate, 0);
    av_opt_set_sample_fmt(swr_ctx, "out_sample_fmt", AV_SAMPLE_FMT_S16, 0);

    if (swr_init(swr_ctx) < 0) {
        cerr << "[AUDIO_PROCESSOR] Could not initialize resampler" << endl;
        av_channel_layout_uninit(&in_ch_layout);
        Cleanup();
        return false;
    }

    av_channel_layout_uninit(&in_ch_layout);

    // Decode and convert audio
    if (!DecodeAudio(audio_data)) {
        Cleanup();
        return false;
    }

    Cleanup();
    return true;
}

bool AudioProcessor::DecodeAudio(AudioData& audio_data) {
    AVPacket* packet = av_packet_alloc();
    AVFrame* frame = av_frame_alloc();

    if (!packet || !frame) {
        cerr << "[AUDIO_PROCESSOR] Could not allocate packet or frame" << endl;
        av_packet_free(&packet);
        av_frame_free(&frame);
        return false;
    }

    audio_data.buffer.clear();

    // Read frames from file
    while (av_read_frame(format_ctx, packet) >= 0) {
        if (packet->stream_index == audio_stream_index) {
            // Send packet to decoder
            int ret = avcodec_send_packet(codec_ctx, packet);
            if (ret < 0) {
                cerr << "[AUDIO_PROCESSOR] Could not send packet to decoder" << endl;
                break;
            }

            // Receive frames from decoder
            while (ret >= 0) {
                ret = avcodec_receive_frame(codec_ctx, frame);
                if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                    break;
                }
                else if (ret < 0) {
                    cerr << "[AUDIO_PROCESSOR] Could not receive frame from decoder" << endl;
                    break;
                }

                // Convert audio format
                ConvertAudioFrame(frame, audio_data);
            }
        }
        av_packet_unref(packet);
    }

    // Flush decoder
    avcodec_send_packet(codec_ctx, nullptr);
    while (avcodec_receive_frame(codec_ctx, frame) >= 0) {
        ConvertAudioFrame(frame, audio_data);
    }

    audio_data.length_samples = audio_data.buffer.size() / (audio_data.channels * sizeof(int16_t));

    av_packet_free(&packet);
    av_frame_free(&frame);
    return true;
}

void AudioProcessor::ConvertAudioFrame(AVFrame* frame, AudioData& audio_data) {
    // Calculate output buffer size
    int out_samples = swr_get_out_samples(swr_ctx, frame->nb_samples);
    int out_buffer_size = av_samples_get_buffer_size(nullptr, audio_data.channels, out_samples, AV_SAMPLE_FMT_S16, 1);

    // Allocate output buffer
    vector<uint8_t> out_buffer(out_buffer_size);
    uint8_t* out_ptr = out_buffer.data();

    // Convert audio 
    int converted_samples = swr_convert(swr_ctx, &out_ptr, out_samples,
        (const uint8_t**)frame->data, frame->nb_samples);

    if (converted_samples > 0) {
        int actual_size = converted_samples * audio_data.channels * sizeof(int16_t);
        audio_data.buffer.insert(audio_data.buffer.end(), out_buffer.begin(), out_buffer.begin() + actual_size);
    }
}
