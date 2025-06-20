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

class Manager
{
public:
	Manager();
	~Manager();

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

#pragma region View


void Manager::MainViewLoop() {
    
}

#pragma endregion Update console screen