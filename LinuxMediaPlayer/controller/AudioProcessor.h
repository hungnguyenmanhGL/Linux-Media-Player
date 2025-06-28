#pragma once
#include "AudioFile.h"
#include "VideoFile.h"
#include <SDL_audio.h>
#include <vector>
#include <iostream>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
#include <libavutil/avutil.h>
#include <libavutil/channel_layout.h>
#include <libavutil/opt.h>
}

struct AudioData {
    vector<uint8_t> buffer;
    int sample_rate;
    int channels;
    SDL_AudioFormat format;
    size_t length_samples;
};

class AudioProcessor {
private:
    AVFormatContext* format_ctx = nullptr;
    AVCodecContext* codec_ctx = nullptr;
    SwrContext* swr_ctx = nullptr;
    int audio_stream_index = -1;

public:
    ~AudioProcessor() {
        Cleanup();
    }

    void Cleanup();

    bool ProcessAudioFile(const std::string& path, AudioData& audio_data);

private:
    bool DecodeAudio(AudioData& audio_data);

    void ConvertAudioFrame(AVFrame* frame, AudioData& audio_data);
};

