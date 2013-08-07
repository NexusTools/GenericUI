#include "cursesmainwindow.h"
#include "cursesmenu.h"

#include <QCoreApplication>

#include <stdio.h>
#include <signal.h>
#include <sys/prctl.h>

CursesMainWindow* CursesMainWindow::_current = 0;
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

void CursesAction::activateCallback() {
    blinkTimer.stop();
    _blink = false;
    markDirty();

    if(!_activateWait)
        return;
    _activateWait = false;

    CursesMenu* parentMenu = qobject_cast<CursesMenu*>(parent());
    if(parentMenu)
        parentMenu->hideChain();

    simEvent(GUIEvent::GUIActivated);
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

/*void CursesMenu::mouseClicked(QPoint p) {
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
}*/

void CursesAction::feedback() {
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

QSize CursesMainWindow::initialScreen() {
    static bool initialized = false;
    if(!initialized) {
        initialized = true;

        signal(SIGWINCH, resetScreen);
        signal(SIGINT, tryTerminate);

        signal(SIGILL, crash);
        signal(SIGSEGV, crash);
        signal(SIGABRT, crash);
        signal(SIGQUIT, crash);
        signal(SIGKILL, crash);
        signal(SIGTERM, crash);
        signal(SIGHUP, crash);

        initscr();
        start_color();
        init_pair(1, COLOR_WHITE, COLOR_BLACK);
        wbkgd(stdscr, COLOR_PAIR(1));

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

void CursesMainWindow::readNextCH()  {
    MEVENT mEvent;

    int ch;
    while((ch = getch()) > 0) {
        qDebug() << "Processing Event" << ch;

        switch(ch) {
            case KEY_MOUSE:
                if(getmouse(&mEvent) == OK)
                    processMouseEvent(mEvent);
                break;

            case KEY_RESIZE:
                resize(checkSize());
                break;

            default:
            {
                GUIWidget* target;

                if(!_windowStack.isEmpty())
                    target = _windowStack.last()->widget();
                else if(ch < 27) {
                    CursesAction::callShortcut(ch);
                    return;
                } else
                    target = this;

                if(target) {
                    qDebug() << target << ch;

                    GUIKeyEvent kEv(ch);
                    target->event(&kEv);
                }
                break;
            }
        }
    }
}

void CursesAction::activate() {
    feedback();

    if(_activateWait)
        return;
    _activateWait = true;
}

bool CursesMenu::processEvent(QEvent *ev) {
    switch(ev->type()) {
        case GUIEvent::GUIMouseClicked:
        {
            if(!QRect(QPoint(0,0),size()).contains(((GUIMouseEvent*)ev)->pos())) {
                hideChain();
                return true;
            }

            break;
        }

        case GUIEvent::GUIKeyTyped:
        {
            if(((GUIKeyEvent*)ev)->key() == 27) {
                hideChain();
                return true;
            }

            break;
        }

        default:
            break;
    }

    return CursesWindow::processEvent(ev);
}

bool CursesMainWindow::processEvent(QEvent *ev) {
    switch(ev->type()) {
        case GUIEvent::GUIMouseClicked:
        {
            if(!_windowStack.isEmpty()) {
                CursesWindow* lastWin = _windowStack.last();
                GUIMouseEvent mEv((GUIEvent::GUIType)((GUIMouseEvent*)ev)->type(),
                                  ((GUIMouseEvent*)ev)->button(), ((GUIMouseEvent*)ev)->pos() - lastWin->geom().topLeft());
                lastWin->widget()->event(&mEv);
                return true;
            }

            break;
        }

        default:
            break;
    }

    return CursesScreen::processEvent(ev);
}

bool CursesAction::processEvent(QEvent *e) {
    switch(e->type()) {
        case GUIEvent::GUIMouseClicked:
            activate();
            break;

        case GUIEvent::GUIStateChanged:
            markDirty();
            break;

        default:
            break;
    }

    return CursesBase::processEvent(e);
}

void cursesDirtyMainWindow() {
    CursesMainWindow* main;
    if((main = CursesMainWindow::current()))
        main->repaint();
}
