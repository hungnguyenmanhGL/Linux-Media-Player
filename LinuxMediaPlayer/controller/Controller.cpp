#include "Controller.h"

Controller::Controller()
{
	manager = MediaManager();
	console = ConsoleView(manager);
}

Controller::~Controller()
{
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
        case 'G': {
            int page = Helper::InputInt(0, console.LastMediaPage());
            console.PrintMediaPage(page, manager);
            break;
        }
        case 'Q': {
            quitSignal = true;
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
            int index = Helper::InputInt(0, manager.PlaylistCount() - 1);
            console.Clear();
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