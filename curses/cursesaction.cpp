#include "cursesmainwindow.h"
#include "cursesaction.h"
#include "cursesmenubar.h"
#include "cursesmenu.h"


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

    emit activated();
    simEvent(GUIEvent::GUIActivated);
}

bool CursesAction::processEvent(QEvent *e) {
    switch(e->type()) {
        case GUIEvent::GUIMouseClicked:
            activate();
            break;

        case GUIEvent::GUIKeyTyped:
            switch(((GUIKeyEvent*)e)->key()) {
                case Qt::Key_Enter:
                    activate();
                    return true;

                case Qt::Key_Tab:
                    if(qobject_cast<GUIMenu*>(parent()) ||
                            qobject_cast<GUIMenuBar*>(parent()))
                        return false;

                default:
                    break;
            }

            break;

        case GUIEvent::GUIFocusChanged:
            markDirty();
            break;

        case GUIEvent::GUIScreenPositionChanged:
            if(!widget()->isFocused())
                break;
        case GUIEvent::GUIFocusGained:
            CursesMainWindow::current()->setCursor(screenX() + _spos, screenY());
            break;

        case GUIEvent::GUIFocusLost:
        {
            CursesMainWindow::current()->setCursor(-1, -1);
            CursesMenu* menu = qobject_cast<CursesMenu*>(parent());
            if(menu && currentFocus()) {
                CursesMenu* newMenu = qobject_cast<CursesMenu*>(currentFocus()->parent());
                while(newMenu) {
                    if(newMenu == menu)
                        break;

                    if(newMenu->currentAction())
                        newMenu = qobject_cast<CursesMenu*>(newMenu->currentAction()->parent());
                }
                if(newMenu == menu)
                    break;

                menu->close();
            }

            break;
        }

        default:
            break;
    }

    return CursesBase::processEvent(e);
}

void CursesAction::activate() {
    feedback();

    if(_activateWait)
        return;
    _activateWait = true;
}

void CursesAction::drawImpl() {
    if(isDisabled()) {
        wbkgd(hnd(), COLOR_PAIR(2));
        wattrset(hnd(), 0);
    } else {
        bool menuOpen = _menu && _menu->isOpen();
        bool standout = menuOpen || wattr().testFlag(Focused);

        if(_blink)
            standout = !standout;

        int attr = standout ? A_STANDOUT : A_NORMAL;
        if(_activateWait || menuOpen)
            attr |= A_BOLD;
        else if(isDisabled())
            attr |= A_DIM;

        wbkgd(hnd(), COLOR_PAIR(1));
        wattrset(hnd(), attr);
    }
    char space = isDisabled() || has_colors() ? ' ' : ACS_CKBOARD;

    wmove(hnd(), 0, 0);
    waddch(hnd(), space);
    foreach(char c, text().toLocal8Bit()) {
        if(c == '_') {
            if(!isDisabled())
                wattron(hnd(), A_UNDERLINE);
            continue;
        } else if(c == ' ')
            c = space;

        waddch(hnd(), c);
        wattroff(hnd(), A_UNDERLINE);
    }
    waddch(hnd(), space);

    int left = width() - 2 - text().length();
    while(left > 0) {
        waddch(hnd(), ' ');
        left--;
    }
}

void CursesAction::feedback() {
    if(isMenuOpen())
        closeMenu();
    else
        openMenu();

    CursesMenu* parentMenu = qobject_cast<CursesMenu*>(parent());
    if(parentMenu)
        parentMenu->showChain();

    blinkTimer.start();
    activateTimer.start(300);
}

