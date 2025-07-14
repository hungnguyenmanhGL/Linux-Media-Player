#pragma once
#include <filesystem>
#include <queue>
#include <vector>
#include <unordered_set>
#include "Playlist.h"
#include "AudioFile.h"
#include "VideoFile.h"

#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <taglib/id3v1tag.h>
#include <taglib/id3v2tag.h>
#include <taglib/mpegfile.h>
#include <taglib/textidentificationframe.h>
#include <taglib/mp4file.h>
#include <taglib/mp4tag.h>

namespace fs = std::filesystem;

extern unordered_set<string> audioExtSet;
extern unordered_set<string> videoExtSet;

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

	bool IsPlaylistNameValid(const string& name);

	/*Add a media file to a playlist,
	playlistIndex is the target playlist's index in playlists, mediaIndex is target media's index in mediaList*/
	void AddMediaToPlaylist(const int& playlistIndex, const int& mediaIndex);

	/*Remove a media file from playlist,
	playlistIndex is the target playlist's index in playlists, mediaIndex is target media's index in that playlist*/
	void RemoveMediaFromPlaylist(const int& playlistIndex, const int& mediaIndex);

	void AddMetadata(const int& plIndex, const int& mediaIndex, const string& key, const string& value);

	void RemoveMetadata(const int& plIndex, const int& mediaIndex, const int& tagIndex);

	void EditDefaultMetadata(const int& plIndex, const int& mediaIndex, MetadataEnum dataEnum);

	void EditCustomMetadata(const int& plIndex, const int& mediaIndex, int index);

	//getters
	int FileCount() { return this->mediaList.size(); }
	int PlaylistCount() { return this->playlists.size(); }

	Playlist& GetPlaylist(const int& index);

	//get media from mediaList
	shared_ptr<MediaFile>& GetMedia(const int& index);

	//get media from a playlist, if plIndex is out of range of playlists -> get media from mediaList
	shared_ptr<MediaFile>& GetMedia(const int& plIndex, const int& mediaIndex);

private:
	vector<shared_ptr<MediaFile>> mediaList;
	vector<Playlist> playlists;
	unordered_set<string> playlistNameSet;
};

