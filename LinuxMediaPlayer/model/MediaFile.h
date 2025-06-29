#pragma once
#include <string>
#include <stdio.h>
#include <unordered_map>
using namespace std;

enum MediaType {
	AUDIO = 0,
	VIDEO = 1
};

enum MetadataEnum {
	TITLE = 0,
	ALBUM = 1,
	ARTIST = 2,
	GENRE = 3,
	PUBLISH_YEAR = 4
};

class MediaFile
{
public:
	MediaFile();
	MediaFile(string path);
	MediaFile(string path, string name, string title, string artist, string album, string genre, int year, int size, int duration);
	virtual ~MediaFile();

	//getters
	string Path() const { return this->path; }

	string Name() { return this->name; }

	int Size() { return this->size; }

	int Duration() { return this->duration; }

	string Title() { return this->title; }

	string Album() { return this->album; }

	string Artist() { return this->artist; }

	string Genre() { return this->genre; }

	int PublishYear() { return this->publishYear; }

	MediaType Type() { return this->type; }

	//setters
	void SetName(const string& name) { this->name = name; }

	void SetTitle(const string& title) { this->title = title; }

	void SetArtist(const string& artist) { this->artist = artist; }

	void SetAlbum(const string& album) { this->album = album; }

	void SetYear(const int& year) { this->publishYear = year;  }

	void SetGenre(const string& genre) { this->genre = genre; }

	virtual void Print();


protected:
	string path;
	string name;
	string title;
	string artist;
	string album;
	string genre;
	int publishYear;
	int size;
	int duration;

	MediaType type;
};