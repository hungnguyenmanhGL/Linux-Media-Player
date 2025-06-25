#pragma once
#include <iostream>
#include <filesystem>
#include <vector>
#include <memory>
#include "MediaFile.h"
using namespace std;

class Playlist
{
public:
	Playlist() {};
	Playlist(const string& name);
	~Playlist() {};

	bool IsMediaInPlaylist(shared_ptr<MediaFile>& entry);

	//getters
	string Name() { return this->name; }
	vector<shared_ptr<MediaFile>>& MediaList() { return this->mediaList; }
	int Count() { return this->mediaList.size(); }

	shared_ptr<MediaFile>& GetMedia(const int& index) { return mediaList[index]; }

	//setter
	void SetName(const string& name) { this->name = name;}

	void AddMedia(shared_ptr<MediaFile>& media);

	void RemoveMedia(const int& index);

	shared_ptr<MediaFile> At(const int& index);

	void Print(bool showContent);

private:
	string name;
	vector<shared_ptr<MediaFile>> mediaList;
};
