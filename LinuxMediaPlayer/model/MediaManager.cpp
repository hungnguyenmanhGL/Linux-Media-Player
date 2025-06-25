#include "MediaManager.h"

unordered_set<string> audioExtSet = { ".mp3", ".wav" };
unordered_set<string> videoExtSet = { ".mp4" };

MediaManager::MediaManager()
{
    playlists.push_back(Playlist("Favorite"));
    playlists.push_back(Playlist("Starred"));
    GetAllMedia(".");
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
                }
                else if (videoExtSet.contains(ext)) {
                    TagLib::FileRef fileRef(entry.path().c_str());
                    TagLib::MP4::Properties* mp4Pro = dynamic_cast<TagLib::MP4::Properties*>(fileRef.audioProperties());
                    if (fileRef.isNull() || fileRef.tag() == nullptr) {
                        cerr << "[TagLib-ERROR] Cannot open file or no tags found for " << entry.path() << endl;
                    }
                    TagLib::Tag* tag = fileRef.tag();

                    shared_ptr<VideoFile> video(new VideoFile(entry.path(), entry.path().filename(),
                        tag->title().toCString(), tag->artist().toCString(),
                        tag->album().toCString(), tag->genre().toCString(), tag->year(),
                        entry.file_size(), fileRef.audioProperties()->lengthInSeconds(),
                        mp4Pro->bitrate(), mp4Pro->codec()));
                    mediaList.push_back(video);
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

Playlist& MediaManager::GetPlaylist(const int& index) {
    return this->playlists[index];
}

shared_ptr<MediaFile>& MediaManager::GetMedia(const int& index) {
    return this->mediaList[index];
}