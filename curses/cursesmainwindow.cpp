#include "cursesmainwindow.h"
#include "cursesdialog.h"
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

void CursesWindow::showImpl() {
    CursesMainWindow::current()->showWindow(this);
}

void CursesWindow::hideImpl() {
    CursesMainWindow::current()->hideWindow(this);
}

bool CursesMenu::isOpen() {
    return CursesMainWindow::current()->isWindowOpen(this);
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

        if(can_change_color()) {
            init_color(COLOR_WHITE, 940, 940, 940);
            init_color(COLOR_BLACK, 282, 314, 325);
            init_color(COLOR_CYAN, 474, 530, 550);
            init_pair(1, COLOR_BLACK, COLOR_WHITE);
            init_pair(2, COLOR_BLACK, COLOR_CYAN);
        } else {
            init_pair(1, COLOR_WHITE, COLOR_BLACK);
            init_pair(2, COLOR_CYAN, COLOR_BLACK);
        }

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

void CursesDialog::center() {
    GUIMainWindow* win = qobject_cast<GUIMainWindow*>(parent());
    if(!win)
        win = mainWindow();
    if(win) // Center this window's content in the main window
        move(win->width()/2 - width()/2,
             win->height()/2 - height()/2 - 2);
}

bool CursesDialog::processEvent(QEvent *ev) {
    switch(ev->type()) {
        case GUIEvent::GUISizeChanged:
        {
            center();
            break;
        }

        case GUIEvent::GUIMouseClicked:
        {
            if(!QRect(QPoint(0,0),size()).contains(((GUIMouseEvent*)ev)->pos())) {
                if(_closable)
                    answer("Okay");
                else {
                    flash();
                    beep();
                }

                return true;
            }

            break;
        }

        case GUIEvent::GUIKeyTyped:
        {
            if(((GUIKeyEvent*)ev)->key() == 27) {
                if(_closable)
                    answer("Okay");
                else {
                    flash();
                    beep();
                }

                return true;
            }

            break;
        }

        default:
            break;
    }

    return CursesWindow::processEvent(ev);
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

        case GUIEvent::GUISizeChanged:
        {
            foreach(CursesWindow* win, _windowStack)
                win->widget()->metaObject()->invokeMethod(win->widget(), "center");

            break;
        }

        default:
            break;
    }

    return CursesScreen::processEvent(ev);
}

void cursesDirtyMainWindow() {
    CursesMainWindow* main;
    if((main = CursesMainWindow::current()))
        main->repaint();
}
