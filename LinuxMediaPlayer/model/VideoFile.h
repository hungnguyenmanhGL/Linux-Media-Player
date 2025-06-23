#pragma once
#include <unordered_map>
#include "MediaFile.h"
#include <mp4properties.h>

extern unordered_map<TagLib::MP4::Properties::Codec, string> codecEnumMap;

class VideoFile
	: public MediaFile
{
public:
	VideoFile();
	VideoFile(string path, string name, string title, string artist, string album, string genre, int year, int size, int duration,
		int bitrate, TagLib::MP4::Properties::Codec codec);
	~VideoFile();

	void Print() override;

	//getters
	int Bitrate() { return this->bitrate; }
	TagLib::MP4::Properties::Codec Codec() const { return this->codec; }

private:
	int bitrate;
	TagLib::MP4::Properties::Codec codec;
};