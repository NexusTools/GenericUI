#include "cursesmainwindow.h"

#include <signal.h>

void resetScreen(int) {
    endwin();
    refresh();
}

void CursesMainWindow::init() {
    static bool initialized = false;
    if(initialized)
        return;
    initialized = true;
    signal(SIGWINCH, resetScreen);
}
