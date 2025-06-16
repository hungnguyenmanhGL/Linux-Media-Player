// LinuxMediaPlayer.cpp : Defines the entry point for the application.
#include "LinuxMediaPlayer.h"
using namespace std;

void ListFilesInDirectory(const string& path) {
	DIR* dir;
	struct dirent* ent;
    // Open the directory
    if ((dir = opendir(path.c_str())) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            string entryName = ent->d_name;
            string fullPath = path + "/" + entryName;

            // Get details on entries
            struct stat entryStat;
            if (stat(fullPath.c_str(), &entryStat) == 0) {
                if (S_ISDIR(entryStat.st_mode)) { //check if entry is directory
                    cout << "[DIR] " << entryName << endl;
                }
                else if (S_ISREG(entryStat.st_mode)) { //check if entry is file
                    cout << "[FILE] " << entryName << endl;
                }
                else { //others
                    cout << "[OTHER] " << entryName << endl;
                }
            }
            else {
                cerr << "Error getting stat for " << fullPath << endl;
            }
        }
        closedir(dir);
    }
    else {
        cerr << "Error: Could not open directory " << path << std::endl;
        perror("opendir");
    }
}

int main()
{
    cout << "Start\n.";
    ListFilesInDirectory(".");
	return 0;
}
