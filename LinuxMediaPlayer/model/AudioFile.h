#pragma once
#include "MediaFile.h"

class AudioFile : public MediaFile
{
public:
	AudioFile();
	AudioFile(string path, string name, int size, int duration);
	~AudioFile();

	//getters
	string TrackName() { return this->trackName; }
	string Album() { return this->album; }
	string Artist() { return this->artist; }
	string Publisher() { return this->publisher; }
	int PublishYear() { return this->publishYear; }

	//setters
	void SetTrackName(const string& name) { this->trackName = name; }
	void SetAlbum(const string& albumName) { this->album = albumName; }
	void SetArtist(const string& artistName) { this->artist = artistName; }
	void SetPublisher(const string& publisher) { this->publisher = publisher; }
	void SetPublishYear(const int& year) { this->publishYear = year; }

private:
	string trackName;
	string album;
	string artist;
	string publisher;
	int publishYear;
};

