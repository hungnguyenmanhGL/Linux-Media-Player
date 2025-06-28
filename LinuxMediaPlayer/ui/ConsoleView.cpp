#include "ConsoleView.h"

ConsoleView::ConsoleView()
{
}

ConsoleView::ConsoleView(MediaManager& manager) {
    entryPerPage = 10;
    curMediaPage = 0;
    entryCnt = manager.FileCount();
    lastMediaPage = entryCnt / entryPerPage - 1;
    if (entryCnt % entryPerPage > 0 || entryCnt == 0) lastMediaPage++;
}

ConsoleView::~ConsoleView()
{
}

void ConsoleView::Clear() {
    //system("clear");
}

void ConsoleView::PrintMediaList(MediaManager& manager) {
    for (int i = 0; i < entryCnt; i++) {
        printf("%d.  %s\n", i + 1, manager.GetMedia(i)->Path().c_str());
    }
}

void ConsoleView::PrintCmdPrompt() {
    switch (state) {
    case ConsoleState::MEDIA_LIST: { ////can call play media from here
        cout << "Viewing list of media. Input command.\n"
            " [P]rev, [N]ext, [G]o to page\n"
            " Show media [D]etails\n"
            " [F]Play media\n"
            " [S]witch to playlist\n"
            " [Q]uit. Command: ";
        break;
    }
    case ConsoleState::PLAYLIST: {
        cout << "Viewing playlists. Input command.\n"
            " [P]rev, [N]ext, [G]o to page\n"
            " Playlist [C]ontent, [U]pdate playlist name\n"
            " [A]dd playlist, [D]elete playlist\n"
            " [S]witch to media\n"
            " [Q]uit. Command: ";
        break;
    }
    case ConsoleState::PLAYLIST_CONTENT: { //can call play media from here
        cout << "Viewing playlist's content. Input command.\n"
            " [P]rev media page, [N]ext media page, [G]o to page\n"
            " [A]dd media, [R]emove media, Show media [D]etails\n"
            " [F]Play media\n"
            " [B]ack. Command: ";
        break;
    }
    case ConsoleState::ADD_MEDIA_PLAYLIST: {
        cout << "Adding media to playlist. Input command.\n"
            " [P]rev media page, [N]ext media page, [G]o to page\n"
            " [A]dd media to playlist\n"
            " Other inputs = Cancel. Command: ";
        break;
    }
    default: {
        cout << "[ERROR] Invalid state.\n";
        break;
    }
    }
}

void ConsoleView::PrintMediaData(MediaManager& manager, const int& index) {
    manager.GetMedia(index)->Print();
}

#pragma region SHOW_MEDIA_PAGES
void ConsoleView::PrintMediaPage(const int& page, MediaManager& manager) {
    int startIndex = page * entryPerPage;
    int cnt = entryPerPage;
    if (startIndex + cnt > entryCnt) cnt = entryCnt % entryPerPage;

    for (int i = 0; i < cnt; i++) {
        int index = startIndex + i;
        if (manager.GetMedia(index)->Type() == MediaType::AUDIO)
            printf("%d. AUDIO - %s\n", index, manager.GetMedia(index)->Name().c_str());
        else 
            printf("%d. VIDEO - %s\n", index, manager.GetMedia(index)->Name().c_str());
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
    lastPlPage = manager.PlaylistCount() / entryPerPage - 1;
    if (manager.PlaylistCount() % entryPerPage > 0 || manager.PlaylistCount() == 0) lastPlPage++;
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

#pragma endregion Print playlists to console

#pragma region PLAYLIST CONTENT
void ConsoleView::PrintPlaylistContentPage(Playlist& pl, const int& page) {
    int startIndex = page * entryPerPage;
    int cnt = entryPerPage;
    if (startIndex + cnt > pl.Count()) cnt = pl.Count() % entryPerPage;

    for (int i = 0; i < cnt; i++) {
        int index = startIndex + i;
        if (pl.GetMedia(index)->Type() == MediaType::AUDIO)
            printf("%d. AUDIO - %s\n", index, pl.GetMedia(index)->Name().c_str());
        else
            printf("%d. VIDEO - %s\n", index, pl.GetMedia(index)->Name().c_str());
    };
    printf("[PLAYLIST] Viewing page %d of %d for playlist %s.\n", page, lastContentPage, pl.Name().c_str());
}

void ConsoleView::CalculatePlaylistContentPages(Playlist& pl) {
    lastContentPage = pl.Count() / entryPerPage - 1;
    if (pl.Count() % entryPerPage > 0 || pl.Count() == 0) lastContentPage++;
}
#pragma endregion Print playlist content
