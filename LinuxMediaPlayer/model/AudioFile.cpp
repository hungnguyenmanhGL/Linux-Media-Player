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