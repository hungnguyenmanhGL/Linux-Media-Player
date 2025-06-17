// LinuxMediaPlayer.cpp : Defines the entry point for the application.
#include "LinuxMediaPlayer.h"
using namespace std;

namespace fs = std::filesystem;

unordered_set<string> extSet{ ".mp3", ".mp4"};

void GetMediaList(const fs::path& path, vector<string>& mediaList) {
    queue<fs::directory_entry> dirList;
    try {
        for (const fs::directory_entry& entry : fs::directory_iterator(path)) {
            if (entry.is_directory()) {
                dirList.push(entry);
            }
            else if (entry.is_regular_file()) {
                if (extSet.contains(entry.path().extension().string())) {
                    mediaList.push_back(entry.path());
                }
            }
        }
    }
    catch (const fs::filesystem_error& e) {
        cerr << "Error accessing dir: " << e.what() << endl;
    }

    while (!dirList.empty()) {
        for (const fs::directory_entry& entry : fs::directory_iterator(dirList.front())) {
            if (entry.is_directory()) {
                cout << "[DIR] " << entry.path() << endl;
                dirList.push(entry);
            }
            else if (entry.is_regular_file()) {
                cout << "[FILE] " << entry.path() << endl;
                if (extSet.contains(entry.path().extension().string())) {
                    mediaList.push_back(entry.path());
                }
            }
        }
        dirList.pop();
    }
}

void ListMediaInDirectory(const string& path) {
    vector<string> mediaList;
    GetMediaList(path, mediaList);
    for (int i = 0; i < mediaList.size(); i++) {
        printf("%d.  %s\n", i + 1, mediaList[i].c_str());
    }

    int cmd;
}

int main()
{
    ListMediaInDirectory(".");
	return 0;
}