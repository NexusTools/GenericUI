#include "cursesmainwindow.h"

#include <signal.h>

CursesMainWindow* CursesMainWindow::_current = 0;

void resetScreen(int) {
    if(CursesMainWindow::current())
        CursesMainWindow::current()->recheckSize();
    else {
        endwin();
        refresh();
    }
}

void CursesMainWindow::init() {
    static bool initialized = false;
    if(initialized)
        return;
    initialized = true;
    signal(SIGWINCH, resetScreen);
}

void cursesDirtyMainWindow() {
    CursesMainWindow* main = CursesMainWindow::current();
    if(main)
        main->notifyDirty();
}
