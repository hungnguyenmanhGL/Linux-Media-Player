#pragma once
#include <iostream>
#include <stdio.h>
#include "MediaManager.h"
using namespace std;

class ConsoleView
{
public:
	ConsoleView();
	ConsoleView(MediaManager& manager);
	~ConsoleView();

	void PrintMediaCmdPrompt();

	void PrintMediaPage(const int& page, MediaManager& manager);

	void PrintCurrentMediaPage(MediaManager& manager);

	void PrintPrevMediaPage(MediaManager& manager);

	void PrintNextMediaPage(MediaManager& manager);

	void PrintPlaylists(MediaManager& manager);

	void PrintMediaData(MediaManager& manager, const int& index);

	//getters
	int LastPage() { return this->lastPage; }
	int CurrentPage() { return this->curPage; }
	int PageCnt() { return this->lastPage + 1; }

private:
	int entryPerPage;
	int curPage;
	int lastPage;
	int entryCnt;

	void PrintMediaList(MediaManager& manager);
};
