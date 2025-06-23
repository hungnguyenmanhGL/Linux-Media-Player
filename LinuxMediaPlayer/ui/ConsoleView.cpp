#include "ConsoleView.h"

ConsoleView::ConsoleView()
{
}

ConsoleView::ConsoleView(MediaManager& manager) {
    entryPerPage = 10;
    curPage = 0;
    entryCnt = manager.FileCount();
    lastPage = entryCnt / entryPerPage - 1;
    if (entryCnt % entryPerPage > 0) lastPage++;
}

ConsoleView::~ConsoleView()
{
}

void ConsoleView::PrintMediaList(MediaManager& manager) {
    for (int i = 0; i < entryCnt; i++) {
        printf("%d.  %s\n", i + 1, manager.MediaList()[i]->Path().c_str());
    }
}

void ConsoleView::PrintMediaCmdPrompt() {
    cout << "Input command ([P]rev, [N]ext, [G]o to page, [Q]uit): ";
}

#pragma region SHOW_MEDIA_PAGES
void ConsoleView::PrintMediaPage(const int& page, MediaManager& manager) {
    int startIndex = page * entryPerPage;
    int cnt = entryPerPage;
    if (startIndex + cnt > entryCnt) cnt = entryCnt % entryPerPage;

    for (int i = 0; i < cnt; i++) {
        int index = startIndex + i;
        printf("%d.  %s\n", index, manager.MediaList()[i]->Name().c_str());
    };
    printf("Viewing page %d of %d.\n", page, lastPage);
}

void ConsoleView::PrintCurrentMediaPage(MediaManager& manager) {
    PrintMediaPage(curPage, manager);
}

void ConsoleView::PrintPrevMediaPage(MediaManager& manager) {
    if (curPage == 0) curPage = lastPage;
    else curPage--;
    PrintCurrentMediaPage(manager);
}

void ConsoleView::PrintNextMediaPage(MediaManager& manager) {
    if (curPage == lastPage) curPage = 0;
    else curPage++;
    PrintCurrentMediaPage(manager);
}
#pragma endregion Print media pages to console

void ConsoleView::PrintPlaylists(MediaManager& manager) {
    for (int i = 0; i < manager.Playlists().size(); i++) {
        cout << i << ". ";
        manager.Playlists()[i].Print(true);
    }
}
