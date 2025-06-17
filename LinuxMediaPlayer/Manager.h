#pragma once
#include <iostream>
#include <string>
#include <filesystem>
#include <stdio.h>
#include <vector>
#include <queue>
#include <unordered_set>
#include "Helper.h"
using namespace std;
namespace fs = std::filesystem;

unordered_set<string> extSet{ ".mp3", ".mp4" };

class Manager
{
public:
	Manager();
	~Manager();

    //get all media files with extension ".mp3" or ".mp4"
	void GetAllMedia(const fs::path& path);

    //print all files in mediaList vector
    void PrintMediaList();

    //update view of main menu (viewing all media files)
    void MainViewLoop();

private:
    vector<string> mediaList;
    int curPage;
    int entryPerPage;

    bool testing = true;

    void PrintCmdPrompt();
    void PrintPage(const int& page);
};

Manager::Manager()
{
    GetAllMedia(".");
    curPage = 0;
    entryPerPage = 10;
}

Manager::~Manager()
{
}

void Manager::PrintCmdPrompt() {
    cout << "Input command ([P]rev, [N]ext, [G]o to page, [Q]uit): ";
}

void Manager::GetAllMedia(const fs::path& path) {
    queue<fs::directory_entry> dirList;
    try {
        fs::directory_entry cur(path);
        dirList.push(cur);
    }
    catch (const fs::filesystem_error& e) {
        cerr << "Error accessing dir: " << e.what() << endl;
    }

    while (!dirList.empty()) {
        for (const fs::directory_entry& entry : fs::directory_iterator(dirList.front())) {
            if (entry.is_directory()) {
                //cout << "[DIR] " << entry.path() << endl;
                dirList.push(entry);
            }
            else if (entry.is_regular_file()) {
                //cout << "[FILE] " << entry.path() << endl;
                if (extSet.contains(entry.path().extension().string())) {
                    mediaList.push_back(entry.path());
                }

                if (testing) mediaList.push_back(entry.path());
            }
        }
        dirList.pop();
    }
    cout << "Total file count: " << mediaList.size() << endl;
}

void Manager::PrintMediaList() {
    for (int i = 0; i < mediaList.size(); i++) {
        printf("%d.  %s\n", i + 1, mediaList[i].c_str());
    }
}

void Manager::MainViewLoop() {
    PrintPage(0);
    int lastPage = mediaList.size() / entryPerPage - 1;
    if (mediaList.size() % entryPerPage > 0) lastPage++;
    char cmd;

    do {
        PrintCmdPrompt();
        cin >> cmd;
        cin.ignore();
        cmd = toupper(cmd);

        switch (cmd) {
        case 'Q': {
            return;
        }
        case 'P': {
            if (curPage > 0) curPage--;
            else curPage = lastPage;
            PrintPage(curPage);
            break;
        }
        case 'N': {
            if (curPage == lastPage) curPage = 0;
            else curPage++;
            PrintPage(curPage);
            break;
        }
        case 'G': {
            cout << "Input page to go to. ";
            curPage = Helper::InputInt(0, lastPage);
            PrintPage(curPage);
            break;
        }
        default:
            cout << "Invalid command.\n";
            break;
        }
    } while (cmd != 'Q');
}

void Manager::PrintPage(const int& page) {
    int startIndex = page * entryPerPage;
    int cnt = entryPerPage;
    if (startIndex + cnt > mediaList.size()) cnt = mediaList.size() % entryPerPage;

    for (int i = 0; i < cnt; i++) {
        int index = startIndex + i;
        printf("%d.  %s\n", index, mediaList[i].c_str());
    };
    printf("Viewing page %d.\n", page);
}