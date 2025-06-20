#include "VideoFile.h"

VideoFile::VideoFile()
{
}

VideoFile::VideoFile(string path, string name, int size, int duration)
	: MediaFile(path, name, size, duration) {

}

VideoFile::~VideoFile()
{
}