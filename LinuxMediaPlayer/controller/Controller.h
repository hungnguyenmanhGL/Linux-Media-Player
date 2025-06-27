#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL_FontCache.h>
#include <SDL_image.h>
#include <thread>
#include <atomic>
#include "MediaManager.h"
#include "ConsoleView.h"
#include "Helper.h"
 
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
#include <libavutil/avutil.h>
}

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

	bool isPlaying;
	atomic_bool quitFlag;


	SDL_Texture* LoadTexture(const std::string& path, SDL_Renderer* renderer);

	void InitSDL();
	void QuitSDL();
};