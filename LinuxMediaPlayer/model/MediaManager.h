#pragma once
#include <filesystem>
#include <queue>
#include <vector>
#include <unordered_set>
#include "Playlist.h"
#include "AudioFile.h"
#include "VideoFile.h"

namespace fs = std::filesystem;

unordered_set<string> audioExtSet{ ".mp3", ".wav" };
unordered_set<string> videoExtSet{ ".mp4" };

class MediaManager
{
public:
	MediaManager();
	~MediaManager();

	//get all media files with extensions in audioExtSet + videoExtSet
	void GetAllMedia(const fs::path& path);

	void CreatePlaylist(const string& name);

	void DeletePlaylist(const int& index);

	void UpdatePlaylistName(const int& index, const string& name);

	/*Add a media file to a playlist,
	playlistIndex is the target playlist's index in playlists, mediaIndex is target media's index in mediaList*/
	void AddMediaToPlaylist(const int& playlistIndex, const int& mediaIndex);

	/*Remove a media file from playlist,
	playlistIndex is the target playlist's index in playlists, mediaIndex is target media's index in that playlist*/
	void RemoveMediaFromPlaylist(const int& playlistIndex, const int& mediaIndex);

	//getters
	vector<shared_ptr<MediaFile>> MediaList() { return this->mediaList; }
	vector<Playlist> Playlists() { return this->playlists; }
	int FileCount() { return this->mediaList.size(); }

private:
	vector<shared_ptr<MediaFile>> mediaList;
	vector<Playlist> playlists;
};

