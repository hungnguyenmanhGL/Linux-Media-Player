#include "Controller.h"

Controller::Controller()
{
	manager = MediaManager();
	console = ConsoleView(manager);

    cout << "FFmpeg Version: " << AV_STRINGIFY(LIBAVUTIL_VERSION_MAJOR) << "."
        << AV_STRINGIFY(LIBAVUTIL_VERSION_MINOR) << "."
        << AV_STRINGIFY(LIBAVUTIL_VERSION_MICRO) << endl;
    InitSDL();
}

Controller::~Controller()
{
    QuitSDL();
}

void Controller::InitSDL() {
    SDL_Init(0);
    TTF_Init();
    int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
    IMG_Init(imgFlags);
}

void Controller::QuitSDL() {
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}

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

void Controller::PlayMediaLoop(const int& index) {
    atomic<shared_ptr<MediaFile>> curMedia(manager.GetMedia(index));
    int winWidth = 800;
    int winHeight = 600;

    SDL_Window* window = SDL_CreateWindow("Bare Media Player", 0, 0, winWidth, winHeight, SDL_WINDOW_OPENGL);
    SDL_Event event;
    //create renderer responsible for render graphic on screen
    SDL_Renderer* render = SDL_CreateRenderer(window, 0, 0);

    //prepare FC_text
    string text = curMedia.load()->Name();
    SDL_Color textColor = { 255, 255, 255, 255 };
    FC_Font* font = FC_CreateFont();
    FC_LoadFont(font, render, "/usr/share/fonts/truetype/ubuntu/Ubuntu-M.ttf", 28, textColor, TTF_STYLE_NORMAL);

    //prepare buttons - Play/Pause, Next, Previous
    SDL_Texture* playTexture = LoadTexture("assets/play.png", render);
    SDL_Texture* pauseTexture = LoadTexture("assets/pause.png", render);
    SDL_Rect playPauseRect;
    playPauseRect.w = 60; 
    playPauseRect.h = 60;  
    playPauseRect.x = (winWidth - playPauseRect.w) / 2;
    playPauseRect.y = winHeight - playPauseRect.h - 30;

    bool quit = false;
    while (!quit && !quitFlag.load()) {
        while (SDL_PollEvent(&event)) {
            //can't return because discrepancy in quitting in this thread vs other (player pressed q)
            if (event.type == SDL_QUIT) {
                quit = true;
            }
        }

        if (quit || quitFlag.load()) {
            break;
        }
        SDL_SetRenderDrawColor(render, 0, 0, 0, 255);
        SDL_RenderClear(render);

        SDL_SetRenderDrawColor(render, 255, 255, 255, 255);
        // Fill button rect with white because images are black
        SDL_RenderFillRect(render, &playPauseRect);
        SDL_Texture* playPauseTexture = playTexture;
        if (!isPlaying) playPauseTexture = pauseTexture;
        if (playPauseTexture != nullptr) {
            SDL_RenderCopy(render, playPauseTexture, NULL, &playPauseRect);
        }
        else {
            // Fallback: if texture failed to load, draw a colored rect as error
            SDL_SetRenderDrawColor(render, 255, 0, 0, 255); //Red
            SDL_RenderFillRect(render, &playPauseRect);
        }

        int textWidth = FC_GetWidth(font, "%s", text.c_str());
        int textHeight = FC_GetHeight(font, "%s", text.c_str());
        FC_Draw(font, render, (winWidth - textWidth) / 2, (winHeight - textHeight) / 2, text.c_str());

        SDL_RenderPresent(render);
    }

    SDL_DestroyRenderer(render);
    SDL_DestroyWindow(window);
}

void Controller::PlayMediaFromPlaylistLoop(const int& playlistIndex, const int& mediaIndex) {
    
}

void Controller::MainLoop() {
    console.SwitchState(ConsoleState::MEDIA_LIST);
    console.PrintCurrentMediaPage(manager);
    char cmd;
    bool quitSignal = false;

    do {
        console.PrintCmdPrompt();
        cin >> cmd;
        cin.ignore();
        cmd = toupper(cmd);

        switch (cmd) {
        case 'D': {
            cout << "Input entry index to show details. ";
            int index = Helper::InputInt(0, manager.FileCount() - 1);
            console.PrintMediaData(manager, index);
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

            //if previous one running, end it to start new thread
            if (sdlThread.joinable()) {
                quitFlag.store(true); 
                sdlThread.join();     
                quitFlag.store(false);
            }
            sdlThread = thread(&Controller::PlayMediaLoop,this, index);
            break;
        }
        case 'G': {
            int page = Helper::InputInt(0, console.LastMediaPage());
            console.PrintMediaPage(page, manager);
            break;
        }
        case 'Q': {
            quitSignal = true;
            quitFlag.store(true);
            if (sdlThread.joinable()) { //wait for sdlThread to stop
                sdlThread.join(); // This will block until PlayMediaLoop finishes execution
            }
            cout << "Quitting...\n";
            break;
        }
        case 'P': {
            console.PrintPrevMediaPage(manager);
            break;
        }
        case 'N': {
            console.PrintNextMediaPage(manager);
            break;
        }
        case 'S': {
            console.Clear();
            quitSignal = PlaylistLoop();
            if (!quitSignal) console.PrintMediaPage(0, manager);
            break;
        }
        default:
            cout << "Invalid command.\n";
            break;
        }

        if (quitSignal) return;
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
        cin >> cmd;
        cin.ignore();
        cmd = toupper(cmd);

        switch (cmd) {
        case 'A': { //create a new playlist
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
            break;
        }
        case 'D': { //delete a playlist
            cout << "Input index to delete (-1 = cancel). ";
            int index = Helper::InputInt(-1, manager.PlaylistCount() - 1);
            if (index == -1) break;
            manager.DeletePlaylist(index);

            console.CalculatePlaylistPages(manager, true);
            console.PrintPlaylistByPage(manager, 0);
            break;
        }
        case 'G': { //go to playlist page n
            int page = Helper::InputInt(0, console.LastPlaylistPage());
            console.PrintPlaylistByPage(manager, page);
            break;
        }
        case 'N': { //next page
            console.PrintNextPlaylistPage(manager);
            break;
        }
        case 'U': { //Update playlist name
            int index = Helper::InputInt(0, manager.PlaylistCount() - 1);
            string name = Helper::InputString("Input new name for playlist: ",
                [&](const std::string& nameToCheck) {
                    return manager.IsPlaylistNameValid(nameToCheck);
                });
            manager.UpdatePlaylistName(index, name);
            break;
        }
        case 'P': { //previous page
            console.PrintPrevPlaylistPage(manager);
            break;
        }
        case 'Q': { //quit
            return true;
        }
        case 'S': { //switch back to media view
            console.SwitchState(ConsoleState::MEDIA_LIST);
            cout << "Switching back to media view...\n";
            return false;
        }
        default:
            cout << "Invalid command.\n";
            break;
        }
    } while (cmd != 'Q');
    return false;
}

void Controller::ContentLoop(const int& index) {
    Playlist& curPl = manager.GetPlaylist(index);
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
            AddMediaToPlaylistLoop(curPl);
            console.Seperate();
            //recalculate page number after addition
            console.CalculatePlaylistContentPages(curPl);
            console.PrintPlaylistContentPage(curPl, 0);
            break;
        }
        case 'B': { //back to view media list
            console.Seperate();
            console.SwitchState(ConsoleState::MEDIA_LIST);
            cout << "Returning to media list...\n";
            return;
        }
        case 'D': {
            console.Seperate();
            int index = Helper::InputInt(0, curPl.Count() - 1);
            curPl.At(index)->Print();
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
        default: { //back to view media list
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
                curPl.AddMedia(entry);
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