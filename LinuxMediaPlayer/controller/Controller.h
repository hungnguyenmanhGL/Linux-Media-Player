#pragma once
#include <SDL2/SDL.h>
#include <thread>
#include <atomic>
#include "MediaManager.h"
#include "ConsoleView.h"
#include "Helper.h"

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

	//play media directly from media list view
	void PlayMediaLoop(const int& index);

	//play media index m from playlist index n
	void PlayMediaFromPlaylistLoop(const int& playlistIndex, const int& mediaIndex);

	void OnPlayWindowClosed();

private:
	MediaManager manager;
	ConsoleView console;
	thread sdlThread;
};