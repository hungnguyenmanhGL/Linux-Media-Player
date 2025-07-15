#pragma once
#include <AudioProcessor.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL_FontCache.h>
#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_rect.h>
#include <SDL_image.h>
class SDLController
{
public:
	SDLController();
	~SDLController();

	void SDLInit();
	void SDLQuit();

	void SDLOpenAudio(const AudioData& audioData);
	void SDLCloseAudio();

	void SDLPlayAudio(const AudioData& audioData, bool clearQueue);

	void CreateWindow();

private:
	SDL_AudioDeviceID deviceId;
	SDL_Window* window = nullptr;
	SDL_Renderer* render = nullptr;

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

	int btnWidth = 60;
	int btnHeight = 60;

	void LoadFont(FC_Font*& font, SDL_Renderer*& render, const int& size, SDL_Color color);
	SDL_Texture* LoadTexture(const string& path, SDL_Renderer* renderer);
	void SetupButtonTexture(SDL_Renderer*& render);
	void SetupButtonRect(const int& winWidth, const int& winHeight);
	void RenderButton(SDL_Renderer*& render, SDL_Texture*& texture, SDL_Rect& rect);
	void RenderAllButtons(SDL_Renderer*& render);
};

