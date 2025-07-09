#include "MediaManager.h"
#include "Helper.h"

unordered_set<string> audioExtSet = { ".mp3" };
unordered_set<string> videoExtSet = { ".mp4" };

MediaManager::MediaManager()
{
    playlists.push_back(Playlist("Favorite"));
    playlists.push_back(Playlist("Starred"));
    GetAllMedia("..");
    playlists[0].AddMedia(mediaList[5]);
    playlists[0].AddMedia(mediaList[0]);
    playlists[0].AddMedia(mediaList[12]);
}

MediaManager::~MediaManager()
{
}

void MediaManager::GetAllMedia(const fs::path& path) {
    queue<fs::directory_entry> dirList;
    try {
        fs::directory_entry cur(path);
        dirList.push(cur);
    }
    catch (const fs::filesystem_error& e) {
        cerr << "Error accessing dir: " << e.what() << endl;
    }

    while (!dirList.empty()) {
        for (const fs::directory_entry& entry : fs::directory_iterator(dirList.front())) {
            if (entry.is_directory()) {
                //cout << "[DIR] " << entry.path() << endl;
                dirList.push(entry);
            }
            else if (entry.is_regular_file()) {
                //cout << "[FILE] " << entry.path() << endl;
                string ext = entry.path().extension().string();
                if (audioExtSet.contains(ext)) {
                    TagLib::FileRef fileRef(entry.path().c_str());
                    if (fileRef.isNull() || fileRef.tag() == nullptr) {
                        cerr << "[TagLib-ERROR] Cannot open file or no tags found for " << entry.path() << endl;
                    }
                    TagLib::Tag* tag = fileRef.tag();

                    shared_ptr<AudioFile> audio(new AudioFile(entry.path(), entry.path().filename(), 
                        tag->title().toCString(), tag->artist().toCString(), 
                        tag->album().toCString(), tag->genre().toCString(), tag->year(),
                        entry.file_size(), fileRef.audioProperties()->lengthInSeconds()));
                    mediaList.push_back(audio);

                    //get custom tags, if there are any
                    TagLib::MPEG::File* mpegFile = dynamic_cast<TagLib::MPEG::File*>(fileRef.file());
                    if (!mpegFile) {
                        cout << "failed to cast file to MPEG.\n";
                        continue;
                    }
                    TagLib::ID3v2::Tag* id3v2tag = mpegFile->ID3v2Tag();
                    if (!id3v2tag) {
                        cout << "Could not get or create ID3v2 tag.\n";
                        continue;
                    }
                    TagLib::ID3v2::FrameList customList = id3v2tag->frameList("TXXX");
                    for (TagLib::ID3v2::Frame* frame : customList) {
                        TagLib::ID3v2::UserTextIdentificationFrame* customTextFrame 
                            = dynamic_cast<TagLib::ID3v2::UserTextIdentificationFrame*>(frame);

                        if (customTextFrame) {
                            string tagName = customTextFrame->description().toCString(true);// true for UTF-8
                            string tagValue = customTextFrame->toString().toCString(true);  
                            tagValue = tagValue.substr(tagName.length() + 3);
                            audio->InsertCustomTag(tagName, tagValue);
                        }
                    }
                }
                else if (videoExtSet.contains(ext)) {
                    TagLib::FileRef fileRef(entry.path().c_str());
                    TagLib::MP4::Properties* mp4Pro = dynamic_cast<TagLib::MP4::Properties*>(fileRef.audioProperties());
                    if (fileRef.isNull() || fileRef.tag() == nullptr) {
                        cerr << "[TagLib-ERROR] Cannot open file or no tags found for " << entry.path() << endl;
                    }
                    TagLib::MP4::Tag* tag = dynamic_cast<TagLib::MP4::Tag*>(fileRef.tag());

                    shared_ptr<VideoFile> video(new VideoFile(entry.path(), entry.path().filename(),
                        tag->title().toCString(), tag->artist().toCString(),
                        tag->album().toCString(), tag->genre().toCString(), tag->year(),
                        entry.file_size(), fileRef.audioProperties()->lengthInSeconds(),
                        mp4Pro->bitrate(), mp4Pro->codec()));
                    mediaList.push_back(video);

                    TagLib::MP4::ItemMap itemMap = tag->itemMap();
                    for (auto it = itemMap.begin(); it != itemMap.end(); ++it) {
                        string atomName = it->first.to8Bit();
                        TagLib::MP4::Item& item = it->second;
                        video->InsertCustomTag(atomName, item.toStringList().toString().to8Bit());
                    }
                }
            }
        }
        dirList.pop();
    }
    cout << "Total media file count: " << mediaList.size() << endl;
}

void MediaManager::CreatePlaylist(const string& name) {
    playlists.push_back(Playlist(name));
    playlistNameSet.insert(name);
    printf("Created playlist %s.\n", name.c_str());
}

void MediaManager::DeletePlaylist(const int& index) {
    string delName = playlists[index].Name();
    playlists.erase(playlists.begin() + index);
    playlistNameSet.erase(delName);
    printf("Deleted playlist %s.\n", delName.c_str());
}

void MediaManager::UpdatePlaylistName(const int& index, const string& name) {
    string oldName = playlists[index].Name();
    (playlists.begin() + index)->SetName(name);
    playlistNameSet.erase(oldName);
    playlistNameSet.insert(name);
    printf("Playlist's name changed: [OLD] %s -> [NEW] %s.\n", oldName.c_str(), name.c_str());
}

bool MediaManager::IsPlaylistNameValid(const string& name) {
    bool res = playlistNameSet.contains(name);
    if (res) printf("Playlist %s already exists, try another name.\n", name.c_str());
    return !res;
}

void MediaManager::AddMediaToPlaylist(const int& plIndex, const int& mediaIndex) {
    shared_ptr<MediaFile>& media = mediaList[mediaIndex];
    playlists[plIndex].AddMedia(media);
    printf("Added %s to playlist %s.\n", media->Name().c_str(), playlists[plIndex].Name().c_str());
}

void MediaManager::RemoveMediaFromPlaylist(const int& plIndex, const int& mediaIndex) {
    string mediaName = playlists[plIndex].At(mediaIndex)->Name();
    playlists[plIndex].RemoveMedia(mediaIndex);
    printf("Removed %s from playlist %s.\n", mediaName.c_str(), playlists[plIndex].Name().c_str());
}

void MediaManager::AddMetadata(const int& plIndex, const int& mediaIndex, const string& key, const string& value) {
    shared_ptr<MediaFile>& media = GetMedia(mediaIndex);
    if (plIndex >= 0) media = playlists[plIndex].At(mediaIndex);

    TagLib::FileRef fileRef(media->Path().c_str());
    if (media->Type() == MediaType::AUDIO) {
        TagLib::MPEG::File* mpegFile = dynamic_cast<TagLib::MPEG::File*>(fileRef.file());
        if (!mpegFile) {
            cout << "failed to cast file to MPEG.\n";
        }
        TagLib::ID3v2::Tag* id3v2tag = mpegFile->ID3v2Tag(true);
        if (!id3v2tag) {
            cout << "Could not get or create ID3v2 tag.\n";
        }

        TagLib::ID3v2::UserTextIdentificationFrame* customFrame = new TagLib::ID3v2::UserTextIdentificationFrame();
        customFrame->setDescription(key);
        customFrame->setText(value);
        id3v2tag->addFrame(customFrame);
        fileRef.save();
    }
    else {

    }
    media->InsertCustomTag(key, value);
}

void MediaManager::EditDefaultMetadata(const int& plIndex, const int& mediaIndex, MetadataEnum dataEnum) {
    shared_ptr<MediaFile>& media = GetMedia(mediaIndex);
    if (plIndex >= 0) media = playlists[plIndex].At(mediaIndex);

    TagLib::FileRef fileRef(media->Path().c_str());
    TagLib::Tag* tag = fileRef.tag();
    switch (dataEnum) {
    case MetadataEnum::TITLE: {
        string title = Helper::InputString("Input title: ", nullptr);
        tag->setTitle(title);
        media->SetTitle(title);
        break;
    }
    case MetadataEnum::ALBUM: {
        string album = Helper::InputString("Input album: ", nullptr);
        tag->setAlbum(album);
        media->SetAlbum(album);
        break;
    }
    case MetadataEnum::ARTIST: {
        string artist = Helper::InputString("Input artist: ", nullptr);
        tag->setArtist(artist);
        media->SetArtist(artist);
        break;
    }
    case MetadataEnum::GENRE: {
        string genre = Helper::InputString("Input genre: ", nullptr);
        tag->setGenre(genre);
        media->SetGenre(genre);
        break;
    }
    case MetadataEnum::PUBLISH_YEAR: {
        cout << "Input year. ";
        int year = Helper::InputInt(0, 2025);
        tag->setYear(year);
        media->SetYear(year);
        break;
    }
    default: {
        break;
    }
    }
    fileRef.save();
}

void MediaManager::EditCustomMetadata(const int& plIndex, const int& mediaIndex, int index) {
    shared_ptr<MediaFile>& media = GetMedia(mediaIndex);
    if (plIndex >= 0) media = playlists[plIndex].At(mediaIndex);

    const string& key = media->CustomKey(index);
    TagLib::FileRef fileRef(media->Path().c_str());
    if (media->Type() == MediaType::AUDIO) {
        TagLib::MPEG::File* mpegFile = dynamic_cast<TagLib::MPEG::File*>(fileRef.file());
        TagLib::ID3v2::Tag* id3v2tag = mpegFile->ID3v2Tag(true);
        TagLib::ID3v2::UserTextIdentificationFrame* customFrame = new TagLib::ID3v2::UserTextIdentificationFrame();

        TagLib::ID3v2::FrameList customList = id3v2tag->frameList("TXXX");
        for (TagLib::ID3v2::Frame* frame : customList) {
            TagLib::ID3v2::UserTextIdentificationFrame* customTextFrame
                = dynamic_cast<TagLib::ID3v2::UserTextIdentificationFrame*>(frame);

            if (customTextFrame->description() == key) {
                string tagValue = Helper::InputString("Input value for custom tag: ", nullptr);
                customTextFrame->setDescription(key);
                customTextFrame->setText(tagValue);
                media->InsertCustomTag(key, tagValue);
                fileRef.save();
                break;
            }
        }
    }
    else {

    }
}

Playlist& MediaManager::GetPlaylist(const int& index) {
    return this->playlists[index];
}

shared_ptr<MediaFile>& MediaManager::GetMedia(const int& index) {
    return this->mediaList[index];
}

shared_ptr<MediaFile>& MediaManager::GetMedia(const int& plIndex, const int& mediaIndex) {
    if (plIndex >= 0 && plIndex < playlists.size()) {
        return playlists[plIndex].At(mediaIndex);
    }
    return GetMedia(mediaIndex);
}