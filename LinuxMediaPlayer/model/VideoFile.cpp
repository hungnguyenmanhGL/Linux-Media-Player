#include "VideoFile.h"
#include <taglib/fileref.h>
#include <taglib/mp4file.h>
#include <taglib/mp4tag.h>
#include <iostream>

unordered_map<TagLib::MP4::Properties::Codec, string> codecEnumMap{
	{TagLib::MP4::Properties::Unknown, "Unknown"},
	{TagLib::MP4::Properties::AAC, "AAC"},
	{TagLib::MP4::Properties::ALAC, "ALAC"}
};

VideoFile::VideoFile()
{
}

VideoFile::VideoFile(string path, string name, string title, string artist, string album, string genre, int year, int size, int duration, 
	int bitrate, TagLib::MP4::Properties::Codec codec)
	: MediaFile(path, name, title, artist, album, genre, year, size, duration), bitrate(bitrate), codec(codec) {
	this->type = MediaType::VIDEO;
}

VideoFile::~VideoFile()
{
}

void VideoFile::Print() {
	MediaFile::Print();
	printf(" Bitrate: %d\n"
		" Codec: %s\n", bitrate, codecEnumMap[codec].c_str());
	printf("Custom: \n");
	for (auto pair : customDataMap) {
		printf(" %s: %s\n", pair.first.c_str(), pair.second.c_str());
	}

	TagLib::FileRef fileRef(path.c_str());
	TagLib::MP4::Tag* tag = dynamic_cast<TagLib::MP4::Tag*>(fileRef.tag());

	TagLib::MP4::ItemMap itemMap = tag->itemMap();
	for (auto it = itemMap.begin(); it != itemMap.end(); it++) {
		cout << it->first.to8Bit(true) << ": " << it->second.toStringList().toString().to8Bit(true) << endl;
	}
}
