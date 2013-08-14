#include "cursesmainwindow.h"
#include "cursesbuttonbox.h"
#include "cursesbutton.h"

CursesButton::CursesButton(QString text, WAttrs attr, GUIContainer *par) : GUIButton(text, attr, par) {
    fitToContent();

    _default = false;
    if(par) {
        CursesButtonBox* mBar = qobject_cast<CursesButtonBox*>(par);
        if(mBar && mBar->findChild<CursesButton*>() == this)
            _default = true;
    }

    _spos = text.indexOf('_');
    if(_spos > -1)
        _shortcut = (Qt::Key)(Qt::Key_A + (text.toLocal8Bit().toLower().at(_spos+1) - 'a'));
    else
        _shortcut = (Qt::Key)0;
    _spos+=2;

    blinkTimer.setInterval(100);
    connect(&blinkTimer, SIGNAL(timeout()), this, SLOT(blink()));

    activateTimer.setSingleShot(true);
    connect(&activateTimer, SIGNAL(timeout()), this, SLOT(activateCallback()));

    _activateWait = false;
    _blink = false;
}

void CursesButton::activateCallback() {
    _activateWait = false;
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

void CursesButton::drawImpl() {
    int attr;

    if(isDisabled()) {
        wbkgd(hnd(), COLOR_PAIR(2));
        wattrset(hnd(), attr = 0);
    } else {
        bool standout = wattr().testFlag(Focused);

        if(_blink)
            standout = !standout;

        attr = standout ? A_STANDOUT : A_NORMAL;
        if(_activateWait)
            attr |= A_BOLD;
        else if(isDisabled())
            attr |= A_DIM;

        wbkgd(hnd(), COLOR_PAIR(1));
        wattrset(hnd(), attr);
    }
    char space = isDisabled() || has_colors() ? ' ' : ACS_CKBOARD;

    wmove(hnd(), 0, 0);
    if(_default)
        wattron(hnd(), A_STANDOUT);
    waddch(hnd(), '[');
    if(_default)
        wattrset(hnd(), attr);
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

    int left = width() - 3 - text().length();
    while(left > 0) {
        waddch(hnd(), space);
        left--;
    }
    waddch(hnd(), space);
    if(_default)
        wattron(hnd(), A_STANDOUT);
    waddch(hnd(), ']');
    if(_default)
        wattrset(hnd(), attr);
}

bool CursesButton::processEvent(QEvent *e) {
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
            CursesButtonBox* btnBox = qobject_cast<CursesButtonBox*>(parent());
            if(kEv->key() == Qt::Key_Space ||
                    (btnBox && (kEv->key() == Qt::Key_Enter))) {
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

void CursesButton::activate() {
    feedback();

    if(_activateWait)
        return;
    _activateWait = true;
}

void CursesButton::feedback() {
    blinkTimer.start();
    activateTimer.start(300);
}
