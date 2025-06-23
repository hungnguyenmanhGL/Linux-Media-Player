#include "MediaFile.h"

MediaFile::MediaFile() {}

MediaFile::MediaFile(string path, string name, string title, string artist, string album, string genre, int year, int size, int duration)
	: path(path), name(name), title(title), artist(artist), album(album), genre(genre), publishYear(year), size(size), duration(duration) {
}

MediaFile::~MediaFile() {}

void MediaFile::Print() {
	printf("File:\n"
		" Path: %s\n"
		" Name: %s\n"
		" Size: %d bytes\n"
		" Duration: %d secs\n", path.c_str(), name.c_str(), size, duration);
	printf("Metadata: \n"
		" Title: %s\n"
		" Artist: %s\n"
		" Album: %s\n"
		" Genre: %s\n"
		" Publish year: %d\n", title.c_str(), artist.c_str(), album.c_str(), genre.c_str(), publishYear);
}