#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL_FontCache.h>
#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_rect.h>
#include <SDL_image.h>
#include <thread>
#include <atomic>
#include "MediaManager.h"
#include "ConsoleView.h"
#include "Helper.h"
#include "AudioProcessor.h"

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

private:
	MediaManager manager;
	ConsoleView console;
	AudioProcessor processor;

	thread sdlThread;
	thread audioThread;

	atomic_bool isPlaying;
	atomic_bool quitFlag;
	atomic_bool windowCloseFlag;

	bool sdlInit;
	bool ttfInit;
	bool imgInit;

	int curPlaylistIndex;
	int curMediaIndex;

	int btnWidth = 60;
	int btnHeight = 60;

	SDL_Texture* LoadTexture(const std::string& path, SDL_Renderer* renderer);

	void InitSDL();
	void QuitSDL();

	void PlayAudio(const string& path);
	void StopAudio();
};