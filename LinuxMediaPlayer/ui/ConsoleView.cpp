#include "ConsoleView.h"

ConsoleView::ConsoleView()
{
}

ConsoleView::ConsoleView(MediaManager& manager) {
    entryPerPage = 10;
    curMediaPage = 0;
    entryCnt = manager.FileCount();
    lastMediaPage = entryCnt / entryPerPage - 1;
    if (entryCnt % entryPerPage > 0) lastMediaPage++;
}

ConsoleView::~ConsoleView()
{
}

void ConsoleView::Clear() {
    system("clear");
}

void ConsoleView::PrintMediaList(MediaManager& manager) {
    for (int i = 0; i < entryCnt; i++) {
        printf("%d.  %s\n", i + 1, manager.MediaList()[i]->Path().c_str());
    }
}

void ConsoleView::PrintMediaCmdPrompt() {
    switch (state) {
    case ConsoleState::MEDIA_LIST: {
        cout << "Input command.\n"
            " [P]rev, [N]ext, [G]o to page\n"
            " Show media [D]etails\n"
            " [S]witch to playlist\n"
            " [Q]uit. Command: ";
        break;
    }
    case ConsoleState::PLAYLIST: {
        cout << "Input command.\n"
            " [P]rev, [N]ext, [G]o to page\n"
            " Playlist [C]ontent, [U]pdate playlist name\n"
            " [A]dd playlist, [D]elete playlist\n"
            " [S]witch to media\n"
            " [Q]uit. Command: ";
        break;
    }
    case ConsoleState::PLAYLIST_CONTENT: {
        cout << "Input command.\n"
            " [P]rev media page, [N]ext media page, [G]o to page\n"
            " [A]dd media, [R]emove media, Show media [D]etails\n"
            " [B]ack. Command: ";
        break;
    }
    default: {
        cout << "[ERROR] Invalid state.\n";
        break;
    }
    }
}

void ConsoleView::PrintMediaData(MediaManager& manager, const int& index) {
    manager.MediaList()[index]->Print();
}

#pragma region SHOW_MEDIA_PAGES
void ConsoleView::PrintMediaPage(const int& page, MediaManager& manager) {
    int startIndex = page * entryPerPage;
    int cnt = entryPerPage;
    if (startIndex + cnt > entryCnt) cnt = entryCnt % entryPerPage;

    for (int i = 0; i < cnt; i++) {
        int index = startIndex + i;
        if (manager.MediaList()[index]->Type() == MediaType::AUDIO)
            printf("%d. AUDIO - %s\n", index, manager.MediaList()[index]->Name().c_str());
        else 
            printf("%d. VIDEO - %s\n", index, manager.MediaList()[index]->Name().c_str());
    };
    printf("Viewing page %d of %d.\n", page, lastMediaPage);
}

void ConsoleView::PrintCurrentMediaPage(MediaManager& manager) {
    PrintMediaPage(curMediaPage, manager);
}

void ConsoleView::PrintPrevMediaPage(MediaManager& manager) {
    curMediaPage--;
    if (curMediaPage < 0) curMediaPage = lastMediaPage;
    PrintCurrentMediaPage(manager);
}

void ConsoleView::PrintNextMediaPage(MediaManager& manager) {
    curMediaPage++;
    if (curMediaPage > lastMediaPage) curMediaPage = 0;
    PrintCurrentMediaPage(manager);
}
#pragma endregion Print media pages to console

#pragma region SHOW_PLAYLIST
void ConsoleView::PrintAllPlaylists(MediaManager& manager) {
    for (int i = 0; i < manager.Playlists().size(); i++) {
        cout << i << ". ";
        manager.Playlists()[i].Print(true);
    }
}

void ConsoleView::CalculatePlaylistPages(MediaManager& manager, bool reset) {
    if (reset) curPlPage = 0;
    lastPlPage = manager.PlaylistCount() / entryPerPage;
    if (manager.PlaylistCount() % entryPerPage > 0) lastPlPage++;
}

void ConsoleView::PrintPlaylistByPage(MediaManager& manager, const int& page) {
    curPlPage = page;
    int startIndex = page * entryPerPage;
    int cnt = entryPerPage;
    if (startIndex + cnt > manager.PlaylistCount()) cnt = manager.PlaylistCount() % entryPerPage;
    for (int i = 0; i < cnt; i++) {
        int index = startIndex + i;
        cout << index << ". ";
        manager.Playlists()[index].Print(false);
    };
    printf("[PLAYLISTS-ALL] Viewing page %d of %d.\n", page, lastPlPage);
}

void ConsoleView::PrintPrevPlaylistPage(MediaManager& manager) {
    curPlPage--;
    if (curPlPage < 0) curPlPage = lastPlPage;
    PrintPlaylistByPage(manager, curPlPage);
}

void ConsoleView::PrintNextPlaylistPage(MediaManager& manager) {
    curPlPage++;
    if (curPlPage > lastPlPage) curPlPage = 0;
    PrintPlaylistByPage(manager, curPlPage);
}

void ConsoleView::PrintPlaylistContentPage(Playlist& pl, const int& page, const int& lastPage) {
    int startIndex = page * entryPerPage;
    int cnt = entryPerPage;
    if (startIndex + cnt > pl.Count()) cnt = pl.Count() % entryPerPage;

    for (int i = 0; i < cnt; i++) {
        int index = startIndex + i;
        if (pl.MediaList()[index]->Type() == MediaType::AUDIO)
            printf("%d. AUDIO - %s\n", index, pl.MediaList()[index]->Name().c_str());
        else
            printf("%d. VIDEO - %s\n", index, pl.MediaList()[index]->Name().c_str());
    };
    printf("[PLAYLIST] Viewing page %d of %d for playlist %.\n", page, lastPage);
}

#pragma endregion Print playlists to console

