#include "cursesmainwindow.h"
#include "cursesdialog.h"
#include "cursesmenubar.h"
#include "cursesbuttonbox.h"
#include "cursesmenu.h"

#include <QCoreApplication>
#include <QTime>

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
    if(mainWin) {
        static int lastSig = 0;
        static QTime lastSigTime;

        if(sig != lastSig || lastSigTime.elapsed() > 150)
            mainWin->terminateRequested(sig);
        lastSigTime.start();
        lastSig = sig;
    } else
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

        ESCDELAY = 25;
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

Qt::Key kOff(Qt::Key k, int o) {
    return (Qt::Key)(k+o);
}

void CursesMainWindow::readNextCH()  {
    MEVENT mEvent;

    int ch;
    static bool altPressed = false;
    forever {
        ch = getch();
        if(!altPressed && ch == ERR)
            return;

        qDebug() << "Processing Event" << ch;
        switch(ch) {
            case 27:
                altPressed = true;
            continue;

            case KEY_MOUSE:
                if(getmouse(&mEvent) == OK)
                    processMouseEvent(mEvent);
                break;

            case KEY_RESIZE:
                resize(checkSize());
                break;

            default:
            {
                CursesBase* target;

                if(currentFocus())
                    target = currentFocus()->internal<CursesBase>();
                else if(!_windowStack.isEmpty())
                    target = _windowStack.last();
                else
                    target = internal<CursesWindow>();

                if(target) {
                    qDebug() << target << ch;

                    Qt::Key key;
                    Qt::KeyboardModifiers mod;

                    /*if(ch > 0 && ch < 27) {
                        mod = Qt::ControlModifier;
                        key = (Qt::Key)(Qt::Key_A + (ch - 1));
                    } else */if(ch >= 97 && ch <= 122)
                        key = kOff(Qt::Key_A, ch - 97);
                    else if(ch >= 65 && ch <= 90) {
                        mod = Qt::ShiftModifier;
                        key = kOff(Qt::Key_A, ch - 65);
                    } else if(ch >= KEY_F(1) && ch <= KEY_F(12))
                        key = kOff(Qt::Key_F1, ch - KEY_F(1));
                    else if(ch >= 48 && ch <= 57)
                        key = kOff(Qt::Key_0, ch - 48);
                    else {
                        switch(ch) {
                            case ERR:
                                key = Qt::Key_Escape;
                                altPressed = false;
                                break;

                            case 353:
                                mod |= Qt::ShiftModifier;
                            case 9:
                                key = Qt::Key_Tab;
                                break;

                            case KEY_ENTER:
                            case 10:
                                key = Qt::Key_Enter;
                                break;

                            case 32:
                                key = Qt::Key_Space;
                                break;

                            case 33:
                                key = Qt::Key_Exclam;
                                break;

                            case 45:
                                key = Qt::Key_Bar;
                                break;

                            case 46:
                                key = Qt::Key_Period;
                                break;

                            case 47:
                                key = Qt::Key_Slash;
                                break;

                            case 58:
                                mod = Qt::ShiftModifier;
                                key = Qt::Key_Colon;
                                break;

                            case 59:
                                key = Qt::Key_Semicolon;
                                break;

                            case 92:
                                key = Qt::Key_Backslash;
                                break;



                            case KEY_UP:
                                key = Qt::Key_Up;
                                break;

                            case KEY_RIGHT:
                                key = Qt::Key_Right;
                                break;

                            case KEY_DOWN:
                                key = Qt::Key_Down;
                                break;

                            case KEY_LEFT:
                                key = Qt::Key_Left;
                                break;

                            case KEY_BACKSPACE:
                                key = Qt::Key_Backspace;
                                break;

                            default:
                                qDebug() << "";
                                return;

                        }
                    }
                    //else if(ch >= ) {

                    //else
                    //    return;


                    if(altPressed)
                        mod |= Qt::AltModifier;

                    GUIKeyEvent kEv(key, mod);
                    target->pushEvent(&kEv);
                }
                break;
            }
        }

        altPressed = false;
    }
}

void CursesDialog::center() {
    GUIMainWindow* win = qobject_cast<GUIMainWindow*>(parent());
    if(!win)
        win = mainWindow();
    if(win) // Center this window's content in the main window
        move(win->width()/2 - width()/2,
             win->height()/2 - height()/2);
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

void CursesAction::openMenu() {
    giveFocus();
    if(_menu)
        _menu->show(QPoint(screenX(), screenY()+1));
}

void CursesAction::closeMenu() {
    if(_menu)
        _menu->hide();
}

bool CursesAction::isMenuOpen() {
    return _menu && _menu->isOpen();
}

bool CursesMenu::processEvent(QEvent *ev) {
    switch(ev->type()) {
        case GUIEvent::GUIMouseClicked:
        {
            if(!QRect(QPoint(0,0),size()).contains(((GUIMouseEvent*)ev)->pos())) {
                CursesWindow::hideImpl();
                GUIMouseEvent* mEv = (GUIMouseEvent*)ev;
                GUIMouseEvent nEv((GUIEvent::GUIType)ev->type(), mEv->button(), mEv->pos() + QPoint(screenX(), screenY()));
                CursesMainWindow::current()->event(&nEv);
                CursesWindow::showImpl();
                hide();

                return true;
            }

            break;
        }

        case GUIEvent::GUIKeyTyped:
        {
            GUIKeyEvent* kEv = (GUIKeyEvent*)ev;
            if(kEv->key() == Qt::Key_Escape) {
                hide();
                return true;
            }

            switch(kEv->key()) {
                case Qt::Key_Tab:
                    return false;

                case Qt::Key_Right:
                case Qt::Key_Left:
                    if(_action)
                        _action->event(kEv);
                    return true;

                case Qt::Key_Down:
                {
                    Children chld = childWidgets();
                    QListIterator<GUIWidget*> i(chld);
                    GUIWidget* first = 0;

                    while(i.hasNext()) {
                        GUIWidget* next = i.next();
                        if(!next->isFocusable())
                            continue;
                        if(!first)
                            first = next;

                        if(next->isFocused()) {
                            while(i.hasNext()) {
                                next = i.next();
                                if(next->isFocusable()) {
                                    next->focus();
                                    return true;
                                }
                            }

                            break;
                        }
                    }

                    first->focus();
                    return true;
                }

                case Qt::Key_Up:
                {
                    Children chld = childWidgets();
                    QListIterator<GUIWidget*> i(chld);
                    GUIWidget* last = 0;
                    i.toBack();

                    while(i.hasPrevious()) {
                        GUIWidget* prev = i.previous();
                        if(!prev->isFocusable())
                            continue;
                        if(!last)
                            last = prev;

                        if(prev->isFocused()) {
                            while(i.hasPrevious()) {
                                prev = i.previous();
                                if(prev->isFocusable()) {
                                    prev->focus();
                                    return true;
                                }
                            }

                            break;
                        }
                    }

                    last->focus();
                    return true;
                }


                default:
                    break;
            }

            foreach(QObject* ch, children()) {
                CursesAction* a = qobject_cast<CursesAction*>(ch);
                if(a && CursesMenuBar::passShortcut(a, kEv->key()))
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

bool CursesMenuBar::processEvent(QEvent* ev) {
    if(ev->type() == GUIEvent::GUIKeyTyped) {
        GUIKeyEvent* kEv = (GUIKeyEvent*)ev;

        switch(kEv->key()) {
            case Qt::Key_Right:
            {
                bool open = false;
                Children chld = childWidgets();
                QListIterator<GUIWidget*> i(chld);
                CursesAction* first = 0;

                while(i.hasNext()) {
                    CursesAction* next = qobject_cast<CursesAction*>(i.next());
                    if(!next || !next->isFocusable())
                        continue;

                    if(!first)
                        first = next;

                    if(next->isMenuOpen() || next->isFocused()) {
                        open = next->isMenuOpen();
                        next->closeMenu();

                        while(i.hasNext()) {
                            next = qobject_cast<CursesAction*>(i.next());
                            if(next && next->isFocusable()) {
                                if(open)
                                    next->openMenu();
                                else
                                    next->giveFocus();
                                return true;
                            }
                        }

                        break;
                    }
                }

                if(open)
                    first->openMenu();
                else
                    first->giveFocus();
                return true;
            }

            case Qt::Key_Left:
            {
                bool open = false;
                Children chld = childWidgets();
                QListIterator<GUIWidget*> i(chld);
                CursesAction* last = 0;
                i.toBack();

                while(i.hasPrevious()) {
                    CursesAction* prev = qobject_cast<CursesAction*>(i.previous());
                    if(!prev || !prev->isFocusable())
                        continue;
                    if(!last)
                        last = prev;

                    if(prev->isMenuOpen() || prev->isFocused()) {
                        open = prev->isMenuOpen();
                        prev->closeMenu();

                        while(i.hasPrevious()) {
                            prev = qobject_cast<CursesAction*>(i.previous());
                            if(prev && prev->isFocusable()) {
                                if(open)
                                    prev->openMenu();
                                else
                                    prev->giveFocus();
                                return true;
                            }
                        }

                        break;
                    }
                }

                if(open)
                    last->openMenu();
                else
                    last->giveFocus();
                return true;
            }

            default:
                break;
        }

        if(passShortcut(kEv->key()))
            return true;
    }

    return CursesBaseContainer::processEvent(ev);
}

bool CursesMenuBar::passShortcut(Qt::Key key) {
    foreach(QObject* ch, children()) {
        CursesAction* a = qobject_cast<CursesAction*>(ch);
        if(a && passShortcut(a, key))
            return true;
    }
}

bool CursesMenuBar::passShortcut(CursesAction* a, Qt::Key key) {
    if(a->isHidden() || a->isDisabled())
        return false;

    if(a->shortcut() == key) {
        a->click();
        return true;
    }

    if(a->_menu)
        foreach(QObject* ch, a->_menu->children()) {
            CursesAction* a = qobject_cast<CursesAction*>(ch);
            if(a && passShortcut(a, key))
                return true;
        }

    return false;
}

void cursesDirtyMainWindow() {
    CursesMainWindow* main;
    if((main = CursesMainWindow::current()))
        main->repaint();
}
