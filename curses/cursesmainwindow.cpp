#include "cursesmainwindow.h"
#include "cursesmenu.h"

#include <QCoreApplication>

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
    show();
}

void CursesMenu::hideChain() {
    if(_action) {
        CursesMenu* parentMenu = qobject_cast<CursesMenu*>(_action->parent());
        if(parentMenu)
            parentMenu->hideChain();
    }
    hide();
}

void CursesAction::activate() {
    blinkTimer.stop();
    _blink = false;
    markDirty();

    if(!_activateWait)
        return;
    _activateWait = false;

    CursesMenu* parentMenu = qobject_cast<CursesMenu*>(parent());
    if(parentMenu)
        parentMenu->hideChain();

    emit activated();
}

void CursesWindow::showImpl() {
    CursesMainWindow::current()->showWindow(this);
}

void CursesWindow::hideImpl() {
    CursesMainWindow::current()->hideWindow(this);
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
    else if(isDisabled())
        attr |= A_DIM;

    wattrset(hnd(), attr);
    wmove(hnd(), 0, 0);
    waddch(hnd(), isDisabled() ? ACS_CKBOARD : ' ');
    foreach(char c, text().toLocal8Bit()) {
        if(c == '_') {
            if(!isDisabled())
                wattron(hnd(), A_UNDERLINE);
            continue;
        } else if(isDisabled() && c == ' ')
            waddch(hnd(), ACS_CKBOARD);
        else {
            waddch(hnd(), c);
            wattroff(hnd(), A_UNDERLINE);
        }
    }
    waddch(hnd(), isDisabled() ? ACS_CKBOARD : ' ');

    int left = width() - 2 - text().length();
    while(left > 0) {
        waddch(hnd(), isDisabled() ? ACS_CKBOARD : ' ');
        left--;
    }
}

void CursesMenu::mouseClicked(QPoint p) {
    if(QRect(QPoint(0,0),size()).contains(p)) {
        QListIterator<GUIWidget*> i(children());

        i.toBack();
        while(i.hasPrevious()) {
           GUIWidget* child = i.previous();
           if(child->isHidden() || child->isDisabled())
               continue;

           CursesBase* curses = child ? child->internal<CursesBase>() : 0;
           if(curses && child->geom().contains(p)) {
               curses->mouseClicked(p - child->geom().topLeft());
               return;
           }
        }
        emit clicked();
        return;
    }

    close();
    CursesMainWindow::current()->mouseClicked(p + geom().topLeft());
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
    printf("Unrecoverable signal %i received.\n%s Terminated.\n\n", sig, qPrintable(QCoreApplication::instance()->applicationName()));

    exit(0);
}

void shutdown(int sig){
    endwin();
    printf("Shutdown signal %i received.\n%s Terminated.\n\n", sig, qPrintable(QCoreApplication::instance()->applicationName()));

    exit(0);
}

void tryTerminate(int sig) {
    CursesMainWindow* mainWin = CursesMainWindow::current();
    if(mainWin)
        mainWin->terminateRequested(sig);
    else
        shutdown(sig);
}

void CursesMainWindow::terminateRequested(int sig) {
    shutdown(sig);
}

QSize CursesMainWindow::init() {
    static bool initialized = false;
    if(!initialized) {
        initialized = true;
        signal(SIGWINCH, resetScreen);

        signal(SIGSEGV, crash);
        signal(SIGABRT, crash);
        signal(SIGTERM, crash);

        signal(SIGKILL, tryTerminate);
        signal(SIGQUIT, tryTerminate);
        signal(SIGINT, tryTerminate);

        initscr();
        start_color();

        //_window = newwin(0, 0, 0, 0);
        mousemask(ALL_MOUSE_EVENTS, NULL);
        nodelay(stdscr, true);
        keypad(stdscr, true);
        meta(stdscr, true);
        curs_set(0);
        noecho();
    }

    return QSize(getmaxx(stdscr), getmaxy(stdscr));
}

void cursesDirtyMainWindow() {
    CursesMainWindow* main;
    if((main = CursesMainWindow::current()))
        main->scheduleRepaint();
}
