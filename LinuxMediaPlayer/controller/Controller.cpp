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
    console.PrintCurrentMediaPage(manager);
    char cmd;

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
        case 'Q': {
            return;
        }
        case 'P': {
            console.PrintPrevMediaPage(manager);
            break;
        }
        case 'N': {
            console.PrintNextMediaPage(manager);
            break;
        }
        case 'G': {
            int page = Helper::InputInt(0, console.LastPage());
            console.PrintMediaPage(page, manager);
            break;
        }
        default:
            cout << "Invalid command.\n";
            break;
        }
    } while (cmd != 'Q');
}