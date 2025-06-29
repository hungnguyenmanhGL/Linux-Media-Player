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

	//edit media's metadata, already have console.Seperate()
	void EditMetadata(const int& plIndex, const int& mediaIndex);

private:
	const int MEDIA_PLAYLIST_INDEX = -1;

	MediaManager manager;
	ConsoleView console;
	AudioProcessor processor;

	thread sdlThread;
	thread audioThread;

	atomic_bool isPlaying;
	atomic_bool quitFlag;
	atomic_bool windowCloseFlag;
	atomic_bool prevFlag;
	atomic_bool nextFlag;
	atomic_bool changeMediaFlag;

	bool sdlInit;
	bool ttfInit;
	bool imgInit;

	//strings used to render on SDL Window of current playing media's data
	string playingMediaName;
	string curPlaylistName;
	string playTimeStr;
	string durationStr;

	atomic<int> curPlaylistIndex;
	atomic<int> curMediaIndex;

	int btnWidth = 60;
	int btnHeight = 60;
	int msWait = 50;
	//the number of msWait's count to achieve a full second;
	int fullSecWaitCount;

	SDL_Texture* LoadTexture(const std::string& path, SDL_Renderer* renderer);
	void SetupButtonTexture(SDL_Renderer*& render,
		SDL_Texture*& playTexture, SDL_Texture*& pauseTexture, SDL_Texture*& prevTexture, SDL_Texture*& nextTexture);
	void SetupButtonRect(const int& winWidth, const int& winHeight, SDL_Rect& playPauseRect, SDL_Rect& prevRect, SDL_Rect& nextRect);

	void InitSDL();
	void QuitSDL();

	void PlayAudio(const string& path);
	string GetNextMediaPath();
	string GetPreviousMediaPath();

	//set the indexes of current playing media + playlist, also set the duration + play time
	void SetCurrentPlayingIndex(const int& plIndex, const int& mediaIndex);

	void SetDurationString(const int& duration);
	void SetPlayTimeString(const int& second);
};