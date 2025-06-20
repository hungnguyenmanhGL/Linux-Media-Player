#pragma once
#include "MediaFile.h"

class VideoFile
	: public MediaFile
{
public:
	VideoFile();
	VideoFile(string path, string name, int size, int duration);
	~VideoFile();

	//getters
	int Bitrate() { return this->bitrate; }
	string Codec() { return this->codec; }

	//setters
	void SetBitrate(const int& rate) { this->bitrate = rate; }
	void SetCodec(const int& codec) { this->codec = codec; }

private:
	int bitrate;
	string codec;
};