#include "cursesmainwindow.h"
#include "cursesbutton.h"


void CursesButton::activateCallback() {
    blinkTimer.stop();
    _blink = false;
    markDirty();

    QString txt = text();
    int pos = txt.indexOf('_');
    if(pos > -1)
        txt = txt.remove(pos, 1);

    emit activated();
    emit selected(txt);
    simEvent(GUIEvent::GUIActivated);
}

void CursesButton::drawImpl() {
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

    int left = width() - 2 - text().length();
    while(left > 0) {
        waddch(hnd(), ' ');
        left--;
    }
    waddch(hnd(), ']');
}

bool CursesButton::processEvent(QEvent *e) {
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

void CursesButton::activate() {
    feedback();

    if(_activateWait)
        return;
    _activateWait = true;
}

void CursesButton::feedback() {
    blinkTimer.start();
    activateTimer.start(300);
    CursesMainWindow::current()->setCursor(screenX() + 1, screenY());
}
