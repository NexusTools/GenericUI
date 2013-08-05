#include "cursesmainwindow.h"
#include "cursesmenu.h"

#include <stdio.h>
#include <signal.h>

CursesMainWindow* CursesMainWindow::_current = 0;
CursesBase* CursesBase::_focusBase = 0;
QHash<char, CursesAction*> CursesAction::shortcuts;

void CursesMenu::showChain() {
    if(_action) {
        move(_action->screenX(), _action->screenY()+1);

        CursesMenu* parentMenu = qobject_cast<CursesMenu*>(_action->parent());
        if(parentMenu)
            parentMenu->showChain();
    }
    showImpl();
}

void CursesAction::activate() {
    blinkTimer.stop();
    if(_blink) {
        _blink = false;
        markDirty();
    }

    if(!_activateWait)
        return;
    _activateWait = false;

    CursesMenu* parentMenu = qobject_cast<CursesMenu*>(parent());
    if(parentMenu)
        parentMenu->close();

    emit activated();
}

void CursesMenu::showImpl() {
    CursesMainWindow::current()->showWindow(this);
    if(_action)
        _action->markDirty();
}

void CursesMenu::closeImpl() {
    CursesMainWindow::current()->closeWindow(this);
    if(_action)
        _action->markDirty();
}

bool CursesMenu::isOpen() {
    return CursesMainWindow::current()->isWindowOpen(this);
}

void CursesAction::drawImpl() {
    bool menuOpen = _menu && _menu->isOpen();
    bool standout = menuOpen || wattr().testFlag(Focused);

    if(_blink)
        standout = !standout;

    int attr = standout ? A_STANDOUT : A_NORMAL;
    if(_activateWait || menuOpen)
        attr |= A_BOLD;

    wattrset(hnd(), attr);
    wmove(hnd(), 0, 0);
    waddch(hnd(), ' ');
    foreach(char c, text().toLocal8Bit()) {
        if(c == '_') {
            wattron(hnd(), A_UNDERLINE);
            continue;
        }

        waddch(hnd(), c);
        wattroff(hnd(), A_UNDERLINE);
    }
    waddch(hnd(), ' ');

    int left = width() - 2 - text().length();
    while(left > 0) {
        waddch(hnd(), ' ');
        left--;
    }
}

void CursesAction::clickFeedback() {
    if(_menu)
        _menu->show(QPoint(screenX(), screenY()+1));

    CursesMenu* parentMenu = qobject_cast<CursesMenu*>(parent());
    if(parentMenu)
        parentMenu->showChain();

    blinkTimer.start();
    activateTimer.start(600);
}

void resetScreen(int) {
    if(CursesMainWindow::current())
        CursesMainWindow::current()->recheckSize();
    else {
        endwin();
        refresh();
    }
}

void crash(int sig) {
    endwin();
    printf("Unrecoverable signal %i received.\nNexusCoordinator Terminated.\n\n", sig);

    exit(0);
}

void end(int sig) {
    endwin();
    printf("Shutdown signal %i received.\nNexusCoordinator Terminated.\n\n", sig);

    exit(0);
}

void CursesMainWindow::init() {
    static bool initialized = false;
    if(initialized)
        return;
    initialized = true;
    signal(SIGWINCH, resetScreen);

    signal(SIGSEGV, crash);
    signal(SIGABRT, crash);
    signal(SIGTERM, crash);

    signal(SIGKILL, end);
    signal(SIGQUIT, end);
    signal(SIGINT, end);
}

void cursesDirtyMainWindow() {
    CursesMainWindow* main;
    if((main = CursesMainWindow::current()))
        main->notifyDirty();
}
