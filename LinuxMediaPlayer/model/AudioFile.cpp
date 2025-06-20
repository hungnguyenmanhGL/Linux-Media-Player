#include "AudioFile.h"

AudioFile::AudioFile()
{
}

AudioFile::AudioFile(string path, string name, int size, int duration)
	: MediaFile(path, name, size, duration) {

}

AudioFile::~AudioFile()
{
}