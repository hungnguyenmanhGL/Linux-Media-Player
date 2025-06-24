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
        console.PrintMediaCmdPrompt();
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
        console.PrintMediaCmdPrompt();
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

}