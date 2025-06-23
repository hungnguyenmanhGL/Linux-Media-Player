#include "VideoFile.h"

VideoFile::VideoFile()
{
}

VideoFile::VideoFile(string path, string name, string title, string artist, string album, string genre, int year, int size, int duration)
	: MediaFile(path, name, title, artist, album, genre, year, size, duration) {
	this->type = MediaType::VIDEO;
}

VideoFile::~VideoFile()
{
}