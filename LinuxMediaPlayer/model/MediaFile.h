#pragma once
#include <string>
#include <stdio.h>
using namespace std;

class MediaFile
{
public:
	MediaFile();
	MediaFile(string path);
	MediaFile(string path, string name, int size, int duration);
	virtual ~MediaFile();

	//getters
	string Path() const { return this->path; }

	string Name() { return this->name; }

	int Size() { return this->size; }

	int Duration() { return this->duration; }

	//setters
	void SetName(const string& name) { this->name = name; }

	void Print();


protected:
	string path;
	string name;
	int size;
	int duration;
};