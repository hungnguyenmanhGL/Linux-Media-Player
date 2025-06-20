#include <MediaFile.h>

MediaFile::MediaFile() {}

MediaFile::MediaFile(string path, string name, int size, int duration)
	: path(path), name(name), size(size), duration(duration) {
}

MediaFile::~MediaFile() {}

void MediaFile::Print() {
	printf("Basic:\n"
		" Path: %s\n"
		" Name: %s\n"
		" Size: %d bytes\n"
		" Duration: %d secs\n", path.c_str(), name.c_str(), size, duration);
}