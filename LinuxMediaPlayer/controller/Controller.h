#pragma once
#include "MediaManager.h"
#include "ConsoleView.h"
#include "Helper.h"
#include <SDL2/SDL.h>

class Controller
{
public:
	Controller();
	~Controller();

	void MainLoop();

	//TO: ContentLoop()
	//FROM: MainLoop()
	bool PlaylistLoop();

	//TO: AddMediaToPlaylist()
	//FROM: PlaylistLoop()
	void ContentLoop(const int& plIndex);

	//FROM: ContentLoop()
	void AddMediaToPlaylistLoop(Playlist& pl);

private:
	MediaManager manager;
	ConsoleView console;
};