#include "SDLController.h"

SDLController::SDLController()
{
}

SDLController::~SDLController()
{
}

void SDLController::SDLInit() {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        cerr << "[SDL_INIT] Fail to init SDL_TTF. Error: " << TTF_GetError() << ".\n";
        return;
    }
    sdlInit = true;
    if (TTF_Init() == -1) {
        cerr << "[SDL_INIT] Fail to init SDL_TTF. Quit.\n";
        return;
    }
    ttfInit = true;
    int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
    int initIMGFlags = IMG_Init(imgFlags);
    if (!(initIMGFlags & imgFlags)) {
        cerr << "[SDL_INIT] Fail to init SDL_IMG. Quit.\n";
        return;
    }
    imgInit = true;
}

void SDLController::SDLQuit() {
    if (imgInit) IMG_Quit();
    if (ttfInit) TTF_Quit();
    if (sdlInit) SDL_Quit();
}

#pragma region SDL Audio
void SDLController::SDLOpenAudio(const AudioData& audioData) {
    SDL_AudioSpec ideal, have;
    //declare the ideal audio setup for playing (device audio may not match this)
    SDL_zero(ideal);
    ideal.freq = audioData.sample_rate;
    ideal.format = audioData.format;
    ideal.channels = audioData.channels;
    ideal.samples = 4096;
    ideal.callback = nullptr;

    deviceId = SDL_OpenAudioDevice(nullptr, 0, &ideal, &have, 0);
    if (deviceId == 0) {
        cerr << "Failed to open audio device: " << SDL_GetError() << endl;
        return;
    }
}

void SDLController::SDLCloseAudio() {
    SDL_ClearQueuedAudio(deviceId);
    SDL_CloseAudioDevice(deviceId);
}

void SDLController::SDLPlayAudio(const AudioData& audioData, bool clearQueue = false) {
    if (clearQueue) {
        SDL_ClearQueuedAudio(deviceId);
    }
    SDL_QueueAudio(deviceId, audioData.buffer.data(), audioData.buffer.size());
}
#pragma endregion Prepare + play audio with SDL

void SDLController::CreateWindow() {
    int winWidth = 800;
    int winHeight = 600;
    window = SDL_CreateWindow("Bare Media Player", 0, 0, winWidth, winHeight, SDL_WINDOW_OPENGL);
    SDL_Event event;
    //create renderer responsible for render graphic on screen
    render = SDL_CreateRenderer(window, 0, 0);

    //prepare FC_text
    SDL_Color textColor = { 255, 255, 255, 255 };
    FC_Font* font = nullptr;
    LoadFont(font, render, 28, textColor);
    FC_Font* durationFont = nullptr;
    LoadFont(durationFont, render, 22, textColor);

    SetupButtonTexture(render);
    SetupButtonRect(winWidth, winHeight);


}

#pragma region SDL Window Setup
SDL_Texture* SDLController::LoadTexture(const string& path, SDL_Renderer* renderer) {
    SDL_Texture* newTexture = nullptr;

    // Load image as SDL_Surface
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == nullptr) {
        cerr << "Unable to load image " << path << "! SDL_image Error: " << IMG_GetError() << endl;
        return nullptr;
    }

    newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
    if (newTexture == nullptr) {
        cerr << "Unable to create texture from " << path << "! SDL Error: " << SDL_GetError() << endl;
    }

    SDL_FreeSurface(loadedSurface);
    return newTexture;
}

void SDLController::LoadFont(FC_Font*& font, SDL_Renderer*& render, const int& size, SDL_Color color) {
    font = FC_CreateFont();
    FC_LoadFont(font, render, "/usr/share/fonts/truetype/ubuntu/Ubuntu-M.ttf", size, color, TTF_STYLE_NORMAL);
}

void SDLController::SetupButtonTexture(SDL_Renderer*& render) {
    playTexture = LoadTexture("assets/play.png", render);
    pauseTexture = LoadTexture("assets/pause.png", render);
    prevTexture = LoadTexture("assets/prev.png", render);
    nextTexture = LoadTexture("assets/next.png", render);
    upVolumeTexture = LoadTexture("assets/volume-up.png", render);
    downVolumeTexture = LoadTexture("assets/volume-down.png", render);
}

void SDLController::SetupButtonRect(const int& winWidth, const int& winHeight) {
    playPauseRect.w = btnWidth;
    playPauseRect.h = btnHeight;
    playPauseRect.x = (winWidth - playPauseRect.w) / 2;
    playPauseRect.y = winHeight - playPauseRect.h * 2;

    prevRect.w = btnWidth;
    prevRect.h = btnHeight;
    nextRect.w = btnWidth;
    nextRect.h = btnHeight;
    prevRect.x = playPauseRect.x - btnWidth * 2;
    prevRect.y = playPauseRect.y;
    nextRect.x = playPauseRect.x + btnWidth * 2;
    nextRect.y = playPauseRect.y;

    upVolumeRect.w = btnWidth / 3 * 2;
    upVolumeRect.h = btnHeight / 3 * 2;
    downVolumeRect = upVolumeRect;

    int midX = (winWidth - upVolumeRect.w) / 2;
    upVolumeRect.x = midX + upVolumeRect.w * 2;
    upVolumeRect.y = nextRect.y - upVolumeRect.h * 2;
    downVolumeRect.x = midX - upVolumeRect.w * 2;
    downVolumeRect.y = upVolumeRect.y;
}

void SDLController::RenderButton(SDL_Renderer*& render, SDL_Texture*& texture, SDL_Rect& rect) {
    SDL_RenderFillRect(render, &rect);
    if (texture != nullptr) {
        SDL_RenderCopy(render, texture, NULL, &rect);
    }
    else {
        SDL_SetRenderDrawColor(render, 255, 0, 0, 255);
        SDL_RenderFillRect(render, &rect);
    }
}

void SDLController::RenderAllButtons(SDL_Renderer*& render) {
    SDL_SetRenderDrawColor(render, 255, 255, 255, 255);
    // Fill button rect with white because images are black
    SDL_Texture* playPauseTexture = playTexture;
    if (isPlaying.load()) playPauseTexture = pauseTexture;
    RenderButton(render, playPauseTexture, playPauseRect);
    RenderButton(render, prevTexture, prevRect);
    RenderButton(render, nextTexture, nextRect);
    RenderButton(render, upVolumeTexture, upVolumeRect);
    RenderButton(render, downVolumeTexture, downVolumeRect);
}
#pragma endregion Prepare for SDL_Window rendering
