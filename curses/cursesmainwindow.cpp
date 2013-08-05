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
    showImpl();
}

void CursesMenu::hideChain() {
    if(_action) {
        CursesMenu* parentMenu = qobject_cast<CursesMenu*>(_action->parent());
        if(parentMenu)
            parentMenu->hideChain();
    }
    hideImpl();
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

void CursesMenu::mouseClicked(QPoint p) {
    if(QRect(QPoint(0,0),size()).contains(p)) {
        QListIterator<GUIWidget*> i(children());

        i.toBack();
        while(i.hasPrevious()) {
           GUIWidget* child = i.previous();
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

void end(int sig) {
    endwin();
    printf("Shutdown signal %i received.\n%s Terminated.\n\n", sig, qPrintable(QCoreApplication::instance()->applicationName()));

    exit(0);
}

QSize CursesMainWindow::init() {
    static bool initialized = false;
    if(!initialized) {
        initialized = true;
        signal(SIGWINCH, resetScreen);

        signal(SIGSEGV, crash);
        signal(SIGABRT, crash);
        signal(SIGTERM, crash);

        signal(SIGKILL, end);
        signal(SIGQUIT, end);
        signal(SIGINT, end);

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
