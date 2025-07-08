#include "Controller.h"

Controller::Controller()
{
	console = ConsoleView(manager);
    fullSecWaitCount = 1000 / msWait;

    cout << "FFmpeg Version: " << AV_STRINGIFY(LIBAVUTIL_VERSION_MAJOR) << "."
        << AV_STRINGIFY(LIBAVUTIL_VERSION_MINOR) << "."
        << AV_STRINGIFY(LIBAVUTIL_VERSION_MICRO) << endl;
}

Controller::~Controller()
{
}

void Controller::TerminateThreads() {
    quitFlag.store(true);
    if (sdlThread.joinable()) sdlThread.join();
    if (audioThread.joinable()) audioThread.join();
    quitFlag.store(false);
}

void Controller::InitSDL() {
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

void Controller::QuitSDL() {
    if (imgInit) IMG_Quit();
    if (ttfInit) TTF_Quit();
    if (sdlInit) SDL_Quit();
}

#pragma region PLAY AUDIO
void Controller::PlayAudio(const string& path) {
    AudioData audioData;
    processor.ProcessAudioFile(path, audioData);

    SDL_AudioSpec ideal, have;
    //declare the ideal audio setup for playing (device audio may not match this)
    SDL_zero(ideal);
    ideal.freq = audioData.sample_rate;
    ideal.format = audioData.format;
    ideal.channels = audioData.channels;
    ideal.samples = 4096;
    ideal.callback = nullptr;

    SDL_AudioDeviceID deviceId = SDL_OpenAudioDevice(nullptr, 0, &ideal, &have, 0);
    if (deviceId == 0) {
        cerr << "Failed to open audio device: " << SDL_GetError() << endl;
        return;
    }

    // Queue audio data, ready to play
    SDL_QueueAudio(deviceId, audioData.buffer.data(), audioData.buffer.size());

    int waitCnt = 0;
    int playTime = 0;

    while (!quitFlag.load() && !windowCloseFlag.load()) {
        if (isPlaying.load()) {
            SDL_PauseAudioDevice(deviceId, 0);
        }
        else {
            SDL_PauseAudioDevice(deviceId, 1);
        }

        /*if (changeMediaFlag.load()) {
            string changePath;
            playTime = 0;
            if (curPlaylistIndex == MEDIA_PLAYLIST_INDEX)
                changePath = manager.GetMedia(curMediaIndex)->Path();
            else
                changePath = manager.GetPlaylist(curPlaylistIndex).At(curMediaIndex)->Path();
            processor.ProcessAudioFile(changePath, audioData);
            SDL_ClearQueuedAudio(deviceId);
            SDL_QueueAudio(deviceId, audioData.buffer.data(), audioData.buffer.size());
            changeMediaFlag.store(false);
        }*/

        if (prevFlag.load()) {
            string prevPath = GetPreviousMediaPath();
            playTime = 0;
            processor.ProcessAudioFile(prevPath, audioData);
            SDL_ClearQueuedAudio(deviceId);
            SDL_QueueAudio(deviceId, audioData.buffer.data(), audioData.buffer.size());
            prevFlag.store(false);
        }
        if (nextFlag.load()) {
            string nextPath = GetNextMediaPath();
            playTime = 0;
            processor.ProcessAudioFile(nextPath, audioData);
            SDL_ClearQueuedAudio(deviceId);
            SDL_QueueAudio(deviceId, audioData.buffer.data(), audioData.buffer.size());
            nextFlag.store(false);
        }
        
        if (SDL_GetQueuedAudioSize(deviceId) == 0) {
            string nextPath = GetNextMediaPath();
            playTime = 0;
            processor.ProcessAudioFile(nextPath, audioData);
            SDL_QueueAudio(deviceId, audioData.buffer.data(), audioData.buffer.size());
        }

        if (downVolumeFlag.load()) {
            bool success = DecreaseVolume(1);
            if (!success) cout << "[Controller] Failed to increase volume.\n";
            GetVolumeFromSystem();
            downVolumeFlag.store(false);
        }
        if (upVolumeFlag.load()) {
            bool success = IncreaseVolume(1);
            if (!success) cout << "[Controller] Failed to decrease volume.\n";
            GetVolumeFromSystem();
            upVolumeFlag.store(false);
        }

        SDL_Delay(msWait); //delay before next call to improve performance
        
        //only count time if audio is playing
        if (isPlaying.load()) waitCnt++;
        //if wait count = 1 second -> increase playTime by 1
        if (waitCnt == fullSecWaitCount) {
            waitCnt = 0;
            playTime++;
            SetPlayTimeString(playTime);
        }
    }
    SDL_ClearQueuedAudio(deviceId);
    SDL_CloseAudioDevice(deviceId);
}

string Controller::GetNextMediaPath() {
    int nextIndex = curMediaIndex + 1;
    //if playing directly from media list -> no playlist
    if (curPlaylistIndex == MEDIA_PLAYLIST_INDEX) {
        if (nextIndex >= manager.FileCount()) nextIndex = 0;
        SetCurrentPlayingIndex(curPlaylistIndex, nextIndex);
        return manager.GetMedia(nextIndex)->Path();
    }
    //if playing from a playlist
    else {
        if (nextIndex >= manager.GetPlaylist(curPlaylistIndex).Count()) nextIndex = 0;
        SetCurrentPlayingIndex(curPlaylistIndex, nextIndex);
        return manager.GetPlaylist(curPlaylistIndex).GetMedia(nextIndex)->Path();
    }
}

string Controller::GetPreviousMediaPath() {
    int nextIndex = curMediaIndex - 1;
    //if playing directly from media list -> no playlist
    if (curPlaylistIndex == MEDIA_PLAYLIST_INDEX) {
        if (nextIndex < 0) nextIndex = manager.FileCount() - 1;
        SetCurrentPlayingIndex(curPlaylistIndex, nextIndex);
        return manager.GetMedia(nextIndex)->Path();
    }
    //if playing from a playlist
    else {
        if (nextIndex < 0) nextIndex = manager.GetPlaylist(curPlaylistIndex).Count() - 1;
        SetCurrentPlayingIndex(curPlaylistIndex, nextIndex);
        return manager.GetPlaylist(curPlaylistIndex).GetMedia(nextIndex)->Path();
    }
}

void Controller::SetCurrentPlayingIndex(const int& plIndex, const int& mediaIndex) {
    curPlaylistIndex.store(plIndex);
    curMediaIndex.store(mediaIndex);
    if (plIndex == MEDIA_PLAYLIST_INDEX) {
        playingMediaName = manager.GetMedia(curMediaIndex)->Name();
        curPlaylistName = "";
        SetDurationString(manager.GetMedia(curMediaIndex)->Duration());
    }
    else {
        playingMediaName = manager.GetPlaylist(curPlaylistIndex).GetMedia(curMediaIndex)->Name();
        curPlaylistName = manager.GetPlaylist(curPlaylistIndex).Name();
        SetDurationString(manager.GetPlaylist(curPlaylistIndex).GetMedia(curMediaIndex)->Duration());
    }
    SetPlayTimeString(0);
}

void Controller::SetDurationString(const int& duration) {
    durationStr = Helper::GetAudioDurationString(duration);
}

void Controller::SetPlayTimeString(const int& sec) {
    playTimeStr = Helper::GetAudioDurationString(sec);
}
#pragma endregion Process media files to play on SDL_audio

void Controller::PlayMediaLoop(const int& index) {
    atomic<shared_ptr<MediaFile>> curMedia;
    if (curPlaylistIndex == MEDIA_PLAYLIST_INDEX)
        curMedia.store(manager.GetMedia(index));
    else
        curMedia.store(manager.GetPlaylist(curPlaylistIndex).At(index));

    int winWidth = 800;
    int winHeight = 600;
    SDL_Window* window = SDL_CreateWindow("Bare Media Player", 0, 0, winWidth, winHeight, SDL_WINDOW_OPENGL);
    SDL_Event event;
    //create renderer responsible for render graphic on screen
    SDL_Renderer* render = SDL_CreateRenderer(window, 0, 0);

    //prepare FC_text
    SDL_Color textColor = { 255, 255, 255, 255 };
    FC_Font* font = nullptr;
    LoadFont(font, render, 28, textColor);
    FC_Font* durationFont = nullptr;
    LoadFont(durationFont, render, 22, textColor);

    SetupButtonTexture(render);
    SetupButtonRect(winWidth, winHeight);

    isPlaying.store(true);
    if (audioThread.joinable()) {
        quitFlag.store(true);
        audioThread.join();
        quitFlag.store(false);
    }
    audioThread = thread(&Controller::PlayAudio, this, curMedia.load()->Path());

    GetVolumeFromSystem();
    windowCloseFlag.store(false);
    while (!windowCloseFlag.load() && !quitFlag.load()) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                //cout << "Quit\n";
                windowCloseFlag.store(true);
            }
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                //cout << "Clicking.\n";
                SDL_Point mousePoint = { event.button.x, event.button.y };
                //cout << mousePoint.x << " " << mousePoint.y << endl;
                if (SDL_PointInRect(&mousePoint, &playPauseRect)) {
                    isPlaying = !isPlaying;
                }
                if (SDL_PointInRect(&mousePoint, &prevRect)) {
                    prevFlag.store(true);
                }
                if (SDL_PointInRect(&mousePoint, &nextRect)) {
                    nextFlag.store(true);
                }

                if (SDL_PointInRect(&mousePoint, &downVolumeRect)) {
                    downVolumeFlag.store(true);
                }
                if (SDL_PointInRect(&mousePoint, &upVolumeRect)) {
                    upVolumeFlag.store(true);
                }
            }
        }

        if (windowCloseFlag.load() || quitFlag.load()) {
            isPlaying.store(false);
            if (audioThread.joinable()) {
                audioThread.join();
            }
            break;
        }
        SDL_SetRenderDrawColor(render, 0, 0, 0, 255);
        SDL_RenderClear(render);

        RenderAllButtons(render);

        int textWidth = FC_GetWidth(font, "%s", playingMediaName.c_str());
        int textHeight = FC_GetHeight(font, "%s", playingMediaName.c_str());
        FC_Draw(font, render, (winWidth - textWidth) / 2, (winHeight - textHeight) / 4, playingMediaName.c_str());
        //adjust to put playlist name under media name
        textWidth = FC_GetWidth(font, "%s", curPlaylistName.c_str());
        FC_Draw(font, render, (winWidth - textWidth) / 2, (winHeight - textHeight) / 4 + 40, curPlaylistName.c_str());

        //show play time + duration
        string timeStr = playTimeStr + " / " + durationStr;
        textWidth = FC_GetWidth(durationFont, "%s", timeStr.c_str());
        FC_Draw(durationFont, render, (winWidth - textWidth) / 2, (winHeight - textHeight) / 4 + 80, timeStr.c_str());

        textWidth = FC_GetWidth(durationFont, "%s", to_string(volume).c_str());
        FC_Draw(durationFont, render, (winWidth - textWidth) / 2, upVolumeRect.y, to_string(volume).c_str());

        SDL_RenderPresent(render);
    }

    SDL_DestroyRenderer(render);
    SDL_DestroyWindow(window);
}

void Controller::MainLoop() {
    console.SwitchState(ConsoleState::MEDIA_LIST);
    console.PrintCurrentMediaPage(manager);
    char cmd;
    bool quitSignal = false;

    do {
        console.PrintCmdPrompt();
        cmd = Helper::GetFirstCharInput();
        cmd = toupper(cmd);

        switch (cmd) {
        case 'D': {
            cout << "Input entry index to show details. ";
            int index = Helper::InputInt(0, manager.FileCount() - 1);
            console.Seperate();
            console.PrintMediaData(manager, index);

            console.Seperate();
            console.PrintCurrentMediaPage(manager);
            break;
        }
        case 'E': {
            cout << "Input entry index to edit. ";
            int mediaIndex = Helper::InputInt(0, manager.FileCount() - 1);
            EditMetadata(-1, mediaIndex);
            console.Seperate();
            console.PrintCurrentMediaPage(manager);
            break;
        }
        case 'F': {
            if (manager.FileCount() == 0) {
                cout << "No media to play.\n";
                break;
            }
            cout << "Input index to play media (-1 = cancel). ";
            int index = Helper::InputInt(-1, manager.FileCount() - 1);
            if (index == -1) break;

            SetCurrentPlayingIndex(MEDIA_PLAYLIST_INDEX, index);
            //if previous one running, end it to start new thread
            if (sdlThread.joinable()) {
                quitFlag.store(true); 
                sdlThread.join();     
                quitFlag.store(false);
            }
            sdlThread = thread(&Controller::PlayMediaLoop,this, index);

            console.Seperate();
            console.PrintCurrentMediaPage(manager);
            break;
        }
        
        //navigation block
        case 'G': {
            int page = Helper::InputInt(0, console.LastMediaPage());
            console.Seperate();
            console.PrintMediaPage(page, manager);
            break;
        }
        case 'N': {
            console.Seperate();
            console.PrintNextMediaPage(manager);
            break;
        }
        case 'P': {
            console.Seperate();
            console.PrintPrevMediaPage(manager);
            break;
        }
        //end of navigation block
        
        case 'Q': {
            quitSignal = true;
            break;
        }
        case 'S': {
            quitSignal = PlaylistLoop();
            console.Seperate();
            if (!quitSignal) console.PrintMediaPage(0, manager);
            break;
        }
        default:
            console.Seperate();
            cout << "Invalid command.\n";
            console.PrintCurrentMediaPage(manager);
            break;
        }

        if (quitSignal) {
            quitFlag.store(true);
            if (sdlThread.joinable()) { //wait for sdlThread to stop
                sdlThread.join(); //blocks until PlayMediaLoop finishes execution
            }
            cout << "Quitting...\n";
            return;
        }
    } while (cmd != 'Q');
}

bool Controller::PlaylistLoop() {
    console.SwitchState(ConsoleState::PLAYLIST);
    console.CalculatePlaylistPages(manager, true);
    console.Seperate();
    console.PrintPlaylistByPage(manager, 0);

    char cmd;
    do {
        console.PrintCmdPrompt();
        cmd = Helper::GetFirstCharInput();
        cmd = toupper(cmd);

        switch (cmd) {
        case 'A': { //create a new playlist
            console.Seperate();
            string name = Helper::InputString("Create a new playlist. Input unique name: ", 
                [&](const std::string& nameToCheck) {
                    return manager.IsPlaylistNameValid(nameToCheck);
                });
            manager.CreatePlaylist(name);

            console.CalculatePlaylistPages(manager, true);
            console.PrintPlaylistByPage(manager, 0);
            break;
        }
        case 'C': { //view content of playlist
            cout << "Input playlist index to view content (input -1 = cancel). ";
            int index = Helper::InputInt(-1, manager.PlaylistCount() - 1);
            if (index == -1) break;
            ContentLoop(index);
            console.Seperate();
            console.PrintPlaylistByPage(manager, 0);
            break;
        }
        case 'D': { //delete a playlist
            if (manager.PlaylistCount() == 0) {
                console.Seperate();
                cout << "There is 0 playlist to delete.\n";
                break;
            }

            cout << "Input index to delete (-1 = cancel). ";
            int index = Helper::InputInt(-1, manager.PlaylistCount() - 1);
            if (index == -1) break;
            manager.DeletePlaylist(index);

            console.Seperate();
            console.CalculatePlaylistPages(manager, true);
            console.PrintPlaylistByPage(manager, 0);
            break;
        }

        //page navigation block
        case 'G': { //go to playlist page n
            if (console.LastPlaylistPage() == 0) {
                console.Seperate();
                cout << "There is only 1 page.\n";
                console.PrintPlaylistByPage(manager, 0);
                break;
            }
            int page = Helper::InputInt(0, console.LastPlaylistPage());
            console.Seperate();
            console.PrintPlaylistByPage(manager, page);
            break;
        }
        case 'N': { //next page
            console.Seperate();
            console.PrintNextPlaylistPage(manager);
            break;
        }
        case 'P': { //previous page
            console.Seperate();
            console.PrintPrevPlaylistPage(manager);
            break;
        }
        //end of navigation block

        case 'Q': { //quit -> back to media loop for quit logic
            return true;
        }
        case 'S': { //switch back to media view
            console.SwitchState(ConsoleState::MEDIA_LIST);
            cout << "Switching back to media view...\n";
            return false;
        }
        case 'U': { //Update playlist name
            int index = Helper::InputInt(0, manager.PlaylistCount() - 1);
            string name = Helper::InputString("Input new name for playlist: ",
                [&](const std::string& nameToCheck) {
                    return manager.IsPlaylistNameValid(nameToCheck);
                });
            manager.UpdatePlaylistName(index, name);
            console.Seperate();
            console.PrintPlaylistByPage(manager, 0);
            break;
        }
        default:
            console.Seperate();
            cout << "Invalid command.\n";
            console.PrintPlaylistByPage(manager, 0);
            break;
        }
    } while (cmd != 'Q');
    return false;
}

void Controller::ContentLoop(const int& playlistIndex) {
    Playlist& curPl = manager.GetPlaylist(playlistIndex);
    console.SwitchState(ConsoleState::PLAYLIST_CONTENT);
    console.CalculatePlaylistContentPages(curPl);
    console.Seperate();
    console.PrintPlaylistContentPage(curPl, 0);

    int curPage = 0;
    char cmd;
    do {
        console.PrintCmdPrompt();
        cin >> cmd;
        cin.ignore();
        cmd = toupper(cmd);

        switch (cmd) {
        case 'A': {
            console.Seperate();
            AddMediaToPlaylistLoop(curPl);
            console.Seperate();
            //recalculate page number after addition
            console.CalculatePlaylistContentPages(curPl);
            console.PrintPlaylistContentPage(curPl, 0);
            break;
        }
        case 'B': { //back to view media list
            console.Seperate();
            console.SwitchState(ConsoleState::PLAYLIST);
            cout << "Returning to playlist's list...\n";
            return;
        }
        case 'D': {
            console.Seperate();
            int index = Helper::InputInt(0, curPl.Count() - 1);
            curPl.At(index)->Print();
            break;
        }
        case 'E': {
            cout << "Input media index to edit. ";
            int index = Helper::InputInt(0, curPl.Count() - 1);
            EditMetadata(playlistIndex, index);
            break;
        }
        case 'F': {
            if (curPl.Count() == 0) {
                cout << "No media in the playlist.\n";
                break;
            }
            cout << "Input index to play media (-1 = cancel). ";
            int mediaIndex = Helper::InputInt(-1, curPl.Count() - 1);
            if (mediaIndex == -1) break;

            SetCurrentPlayingIndex(playlistIndex, mediaIndex);
            //if previous one running, end it to start new thread
            if (sdlThread.joinable()) {
                quitFlag.store(true);
                sdlThread.join();
                quitFlag.store(false);
            }
            sdlThread = thread(&Controller::PlayMediaLoop, this, mediaIndex);

            console.Seperate();
            console.PrintPlaylistContentPage(curPl, curPage);
            break;
        }
        //navigate playlist's content
        case 'G': {
            console.Seperate();
            curPage = Helper::InputInt(0, console.LastContentPage());
            console.PrintPlaylistContentPage(curPl, curPage);
            break;
        }
        case 'N': {
            console.Seperate();
            curPage++;
            if (curPage > console.LastContentPage()) curPage = 0;
            console.PrintPlaylistContentPage(curPl, curPage);
            break;
        }
        case 'P': {
            console.Seperate();
            curPage--;
            if (curPage < 0) curPage = console.LastContentPage();
            console.PrintPlaylistContentPage(curPl, curPage);
            break;
        }
        //end of content navigation

        case 'R': { //remove a media from playlist
            console.Seperate();
            if (curPl.Count() == 0) {
                printf("Playlist %s has 0 file.\n", curPl.Name().c_str());
                break;
            }

            console.PrintPlaylistContentPage(curPl, curPage);
            cout << "Input -1 to cancel. ";
            int index = Helper::InputInt(-1, curPl.Count() - 1);
            if (index == -1) {
                cout << "Cancel removal. Back to playlist content.\n";
                break;
            }
            curPl.RemoveMedia(index);
            console.CalculatePlaylistContentPages(curPl);
            console.PrintPlaylistContentPage(curPl, 0);
            break;
        }
        default: {
            console.Seperate();
            cout << "Invalid command. Try again.\n";
            break;
        }
        }
    } while (cmd != 'B');
}

void Controller::AddMediaToPlaylistLoop(Playlist& curPl) {
    console.SwitchState(ConsoleState::ADD_MEDIA_PLAYLIST);
    int curPage = 0;
    console.PrintMediaPage(curPage, manager);

    char cmd;
    do {
        console.PrintCmdPrompt();
        cin >> cmd;
        cin.ignore();
        cmd = toupper(cmd);

        switch (cmd) {
        case 'A': {
            int index = Helper::InputInt(0, manager.FileCount()-1);
            shared_ptr<MediaFile>& entry = manager.GetMedia(index);
            if (curPl.IsMediaInPlaylist(entry)) {
                printf("Media %s is already in playlist %s.\n", 
                    entry->Name().c_str(), curPl.Name().c_str());
            }
            else {
                console.Seperate();
                curPl.AddMedia(entry);
                console.PrintMediaPage(curPage, manager);
            }
            break;
        }
        case 'G': {
            console.Seperate();
            curPage = Helper::InputInt(0, console.LastMediaPage());
            console.PrintMediaPage(curPage, manager);
            break;
        }
        case 'N': {
            console.Seperate();
            curPage++;
            if (curPage > console.LastMediaPage()) curPage = 0;
            console.PrintMediaPage(curPage, manager);
            break;
        }
        case 'P': {
            console.Seperate();
            curPage--;
            if (curPage < 0) curPage = console.LastMediaPage();
            console.PrintMediaPage(curPage, manager);
            break;
        }
        default:
            console.Seperate();
            console.SwitchState(ConsoleState::PLAYLIST_CONTENT);
            cout << "Cancel adding media. Back to view playlist content.\n";
            return;
        }
    } while (cmd != 'B');
}

void Controller::EditMetadata(const int& plIndex, const int& mediaIndex) {
    int dataEnum;
    do {
        console.Seperate();
        shared_ptr<MediaFile>& media = manager.GetMedia(plIndex, mediaIndex);
        media->Print();
        printf("Input index of metadata to edit (Input -1 to cancel):\n"
            " %d = Title, %d = Album , %d = Artist, %d = Genre, %d = Publish year.\n",
            MetadataEnum::TITLE, MetadataEnum::ALBUM, MetadataEnum::ARTIST, MetadataEnum::GENRE, MetadataEnum::PUBLISH_YEAR);
        
        int customCnt = media->CustomDataCount();
        int totalTagCnt = DEFAULT_METADATA_CNT + customCnt;
        if (customCnt > 0) printf("Custom tag(s): \n");
        for (int i = DEFAULT_METADATA_CNT; i < totalTagCnt; i++) {
            if (i == totalTagCnt - 1) printf(" %d = %s.\n", i, media->CustomKey(i - DEFAULT_METADATA_CNT).c_str());
            else printf(" %d = %s, ", i, media->CustomKey(i - DEFAULT_METADATA_CNT).c_str());
        }

        dataEnum = Helper::InputInt(-1, totalTagCnt - 1);
        if (dataEnum >= MetadataEnum::TITLE && dataEnum <= MetadataEnum::PUBLISH_YEAR) 
            manager.EditDefaultMetadata(plIndex, mediaIndex, (MetadataEnum)dataEnum);
        if (dataEnum > MetadataEnum::PUBLISH_YEAR)
            manager.EditCustomMetadata(plIndex, mediaIndex, dataEnum - DEFAULT_METADATA_CNT);

    } while (dataEnum != -1);
}

#pragma region SDL_SETUP
SDL_Texture* Controller::LoadTexture(const string& path, SDL_Renderer* renderer) {
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

void Controller::LoadFont(FC_Font*& font, SDL_Renderer*& render, const int& size, SDL_Color color) {
    font = FC_CreateFont();
    FC_LoadFont(font, render, "/usr/share/fonts/truetype/ubuntu/Ubuntu-M.ttf", size, color, TTF_STYLE_NORMAL);
}

void Controller::SetupButtonTexture(SDL_Renderer*& render) {
    playTexture = LoadTexture("assets/play.png", render);
    pauseTexture = LoadTexture("assets/pause.png", render);
    prevTexture = LoadTexture("assets/prev.png", render);
    nextTexture = LoadTexture("assets/next.png", render);
    upVolumeTexture = LoadTexture("assets/volume-up.png", render);
    downVolumeTexture = LoadTexture("assets/volume-down.png", render);
}

void Controller::SetupButtonRect(const int& winWidth, const int& winHeight) {
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

void Controller::RenderButton(SDL_Renderer*& render, SDL_Texture*& texture, SDL_Rect& rect) {
    SDL_RenderFillRect(render, &rect);
    if (texture != nullptr) {
        SDL_RenderCopy(render, texture, NULL, &rect);
    }
    else {
        SDL_SetRenderDrawColor(render, 255, 0, 0, 255);
        SDL_RenderFillRect(render, &rect);
    }
}

void Controller::RenderAllButtons(SDL_Renderer*& render) {
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
#pragma endregion Setup sdl-related components

#pragma region System Volume
void Controller::GetVolumeFromSystem() {
    FILE* pipe = popen("pactl get-sink-volume @DEFAULT_SINK@ | grep -o '[0-9]*%' | head -1 | tr -d '%'", "r");
    if (!pipe) {
        cerr << "[CONTROLLER] Cannot open system file for volume control.\n";
        return;
    }

    char buffer[16];
    if (fgets(buffer, sizeof(buffer), pipe)) {
        pclose(pipe);
        volume = atoi(buffer);
        return;
    }
    pclose(pipe);
}

//delta is by percentage
bool Controller::IncreaseVolume(int delta) {
    if (volume == 100) return true;
    string command = "pactl set-sink-volume @DEFAULT_SINK@ +" + to_string(delta) + "%";
    return system(command.c_str()) == 0;
}

bool Controller::DecreaseVolume(int delta) {
    if (volume == 0) return true;
    string command = "pactl set-sink-volume @DEFAULT_SINK@ -" + to_string(delta) + "%";
    return system(command.c_str()) == 0;
}
#pragma endregion Adjust volume with system Linux + pactl (pulseaudio-utils)

