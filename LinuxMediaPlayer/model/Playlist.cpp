#include "Playlist.h"

Playlist::Playlist(const string& name) {
	this->name = name;
}

bool Playlist::IsMediaInPlaylist(shared_ptr<MediaFile>& entry) {
	for (shared_ptr<MediaFile>& file : mediaList) {
		if (entry == file) return true;
	}
	return false;
}

void Playlist::AddMedia(shared_ptr<MediaFile>& media) {
	mediaList.push_back(media);
	printf("Added %s to playlist %s.\n", media->Name().c_str(), name.c_str());
	printf("Playlist %s has %d entry(s).\n", name.c_str(), Count());
}

void Playlist::RemoveMedia(const int& index) {
	shared_ptr<MediaFile> toRemove = *(mediaList.begin() + index);
	mediaList.erase(mediaList.begin() + index);
	printf("Removed %s from playlist %s.\n", toRemove->Name().c_str(), name.c_str());
	printf("Playlist %s has %d entry(s).\n", name.c_str(), Count());
}

shared_ptr<MediaFile> Playlist::At(const int& index) {
	return this->mediaList[index];
}

void Playlist::Print(bool showContent = false) {
	printf("%s - %d entry(s).\n", name.c_str(), mediaList.size());
	if (showContent) {
		for (int i = 0; i < mediaList.size(); i++) {
			printf(" %d. %s\n", i, mediaList[i]->Name().c_str());
		} cout << "End of playlist.\n" << endl;
	}
}