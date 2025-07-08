#include "AudioFile.h"

AudioFile::AudioFile()
{
}

AudioFile::AudioFile(string path, string name, string title, string artist, string album, string genre, int year, int size, int duration)
	: MediaFile(path, name, title, artist, album, genre, year, size, duration) {
	this->type = MediaType::AUDIO;
}

AudioFile::~AudioFile()
{
}

void AudioFile::Print() {
	MediaFile::Print();
	printf("Custom: \n");
	for (auto pair : customDataMap) {
		printf(" %s: %s\n", pair.first.c_str(), pair.second.c_str());
	}
}