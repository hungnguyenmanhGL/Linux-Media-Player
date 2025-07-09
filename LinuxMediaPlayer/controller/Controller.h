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

	void InitSDL();
	void QuitSDL();
	void TerminateThreads();

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

	//edit metadata loop, REMEMBER to switch back to previous state on the calling function after quitting
	void EditMetadataLoop(const int& plIndex, const int& mediaIndex);

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
	atomic_bool upVolumeFlag;
	atomic_bool downVolumeFlag;

	SDL_Texture* playTexture = nullptr;
	SDL_Texture* pauseTexture = nullptr;
	SDL_Texture* prevTexture = nullptr;
	SDL_Texture* nextTexture = nullptr;
	SDL_Texture* upVolumeTexture = nullptr;
	SDL_Texture* downVolumeTexture = nullptr;

	SDL_Rect playPauseRect;
	SDL_Rect prevRect, nextRect;
	SDL_Rect upVolumeRect, downVolumeRect;

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
	atomic_int volume;

	int btnWidth = 60;
	int btnHeight = 60;

	//miliseconds of wait time between each audio loop, divisible to 1000 (1000ms = 1s)
	int msWait = 25;
	//the number of msWait's count to achieve a full second;
	int fullSecWaitCount;

	//SDL-related function declaration block
	SDL_Texture* LoadTexture(const std::string& path, SDL_Renderer* renderer);
	void LoadFont(FC_Font*& font, SDL_Renderer*& render, const int& size, SDL_Color color);
	void SetupButtonTexture(SDL_Renderer*& render);
	void SetupButtonRect(const int& winWidth, const int& winHeight);
	void RenderButton(SDL_Renderer*& render, SDL_Texture*& texture, SDL_Rect& rect);
	void RenderAllButtons(SDL_Renderer*& render);
	//end of block

	void PlayAudio(const string& path);
	string GetNextMediaPath();
	string GetPreviousMediaPath();
	//set the indexes of current playing media + playlist, also set the duration + play time
	void SetCurrentPlayingIndex(const int& plIndex, const int& mediaIndex);
	void SetDurationString(const int& duration);
	void SetPlayTimeString(const int& second);

	void GetVolumeFromSystem();
	bool IncreaseVolume(int delta);
	bool DecreaseVolume(int delta);
};