#pragma once
#include "MediaFile.h"

class AudioFile : public MediaFile
{
public:
	AudioFile();
	AudioFile(string path, string name, string title, string artist, string album, string genre, int year, int size, int duration);
	~AudioFile();

	void Print() override;

private:
	
};

