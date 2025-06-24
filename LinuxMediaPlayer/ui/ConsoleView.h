#pragma once
#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include "MediaManager.h"
using namespace std;

enum ConsoleState {
	MEDIA_LIST = 0,
	PLAYLIST = 1,
	PLAYLIST_CONTENT = 2
};

class ConsoleView
{
public:
	ConsoleView();
	ConsoleView(MediaManager& manager);
	~ConsoleView();

	void Clear();

	void PrintMediaCmdPrompt();

	void PrintMediaPage(const int& page, MediaManager& manager);

	void PrintCurrentMediaPage(MediaManager& manager);

	void PrintPrevMediaPage(MediaManager& manager);

	void PrintNextMediaPage(MediaManager& manager);

	void PrintMediaData(MediaManager& manager, const int& index);

	void PrintAllPlaylists(MediaManager& manager);

	//calculate page number for playlists. reset = true -> set current playlist page = 0
	void CalculatePlaylistPages(MediaManager& manager, bool reset = false);

	//calculate page number for playlist content. reset = true -> set current content page = 0
	void CalculatePlaylistContentPages(Playlist& pl, bool reset = false);


	//print to console all playlists by page,
	//as playlist number can be changed in runtime, need to re-calculate page number
	void PrintPlaylistByPage(MediaManager& manager, const int& page);

	void PrintPrevPlaylistPage(MediaManager& manager);

	void PrintNextPlaylistPage(MediaManager& manager);



	//playlist's entry number can be changed in runtime, need to re-calculate page number
	void PrintPlaylistContentPage(Playlist& pl, const int& page, const int& lastPage);

	//getters
	int LastMediaPage() { return this->lastMediaPage; }
	int LastPlaylistPage() { return this->lastPlPage; }
	int LastContentPage() { return this->lastContentPage; }

	int CurrentMediaPage() { return this->curMediaPage; }
	int MediaPageCnt() { return this->lastMediaPage + 1; }

	//setters
	void SwitchState(const ConsoleState& state) { this->state = state; }


private:
	int entryPerPage;
	int curMediaPage;
	int lastMediaPage;
	int entryCnt;

	int curPlPage;
	int lastPlPage;

	int curContentPage;
	int lastContentPage;

	ConsoleState state;

	void PrintMediaList(MediaManager& manager);
};
