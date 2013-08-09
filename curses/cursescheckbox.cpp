#include "cursesmainwindow.h"
#include "cursescheckbox.h"


void CursesCheckBox::activateCallback() {
    _activateWait = false;
    _checked = !_checked;
    blinkTimer.stop();
    _blink = false;
    markDirty();

    QString txt = text();
    int pos = txt.indexOf('_');
    if(pos > -1)
        txt = txt.remove(pos, 1);

    emit activated();
    emit selected(QVariant(txt));
    simEvent(GUIEvent::GUIActivated);
}

void CursesCheckBox::drawImpl() {
    if(isDisabled()) {
        wbkgd(hnd(), COLOR_PAIR(2));
        wattrset(hnd(), 0);
    } else {
        bool standout = wattr().testFlag(Focused);

        if(_blink)
            standout = !standout;

        int attr = standout ? A_STANDOUT : A_NORMAL;
        if(_activateWait)
            attr |= A_BOLD;
        else if(isDisabled())
            attr |= A_DIM;

        wbkgd(hnd(), COLOR_PAIR(1));
        wattrset(hnd(), attr);
    }
    char space = isDisabled() || has_colors() ? ' ' : ACS_CKBOARD;

    wmove(hnd(), 0, 0);
    waddch(hnd(), '[');
    waddch(hnd(), _checked ? 'X' : space);
    waddch(hnd(), ']');
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

    int left = width() - 4 - text().length();
    while(left > 0) {
        waddch(hnd(), ' ');
        left--;
    }
}

bool CursesCheckBox::processEvent(QEvent *e) {
    switch(e->type()) {
        case GUIEvent::GUIMouseClicked:
            activate();
            break;

        case GUIEvent::GUIFocusChanged:
            markDirty();
            break;

        case GUIEvent::GUIKeyTyped:
        {
            GUIKeyEvent* kEv = (GUIKeyEvent*)e;
            if(kEv->key() == Qt::Key_Space) {
                activate();
                return true;
            }
            break;
        }

        case GUIEvent::GUIScreenPositionChanged:
            if(!widget()->isFocused())
                break;
        case GUIEvent::GUIFocusGained:
            CursesMainWindow::current()->setCursor(screenX() + _spos, screenY());
            break;

        case GUIEvent::GUIFocusLost:
            CursesMainWindow::current()->setCursor(-1, -1);
            break;

        default:
            break;
    }

    return CursesBase::processEvent(e);
}

void CursesCheckBox::activate() {
    feedback();

    if(_activateWait)
        return;
    _activateWait = true;
}

void CursesCheckBox::feedback() {
    blinkTimer.start();
    activateTimer.start(300);
}

