#include "Playlist.h"

Playlist::Playlist(const string& name) {
	this->name = name;
}

void Playlist::AddMedia(shared_ptr<MediaFile>& media) {
	mediaList.push_back(media);
	printf("Added %d to playlist %d.\n", media->Name().c_str(), name.c_str());
}

void Playlist::RemoveMedia(const int& index) {
	shared_ptr<MediaFile> toRemove = *(mediaList.begin() + index);
	mediaList.erase(mediaList.begin() + index);
	printf("Removed %d from playlist %d.\n", toRemove->Name().c_str(), name.c_str());
}

shared_ptr<MediaFile> Playlist::At(const int& index) {
	return this->mediaList[index];
}

void Playlist::Print() {
	printf("Playlist %s has %d media file(s):\n", name.c_str(), mediaList.size());
	for (int i = 0; i < mediaList.size(); i++) {
		printf(" %d. %s\n", i, mediaList[i]->Name().c_str());
	} cout << "End of playlist.\n" << endl;
}