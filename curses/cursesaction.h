#ifndef CURSESACTION_H
#define CURSESACTION_H

#include <QTimer>

#include <guiaction.h>

#include "cursesbase.h"

class CursesAction : public GUIAction, public CursesBase
{
    Q_OBJECT
    BASIC_CURSES_OBJECT

public:
    inline CursesAction(QString text, GUIContainer* par =0) : GUIAction(text, par) {
        fitToContent();
        if(par)
            CursesBase::updateParent(((GUIWidget*)par)->internal<CursesBase>());

        int pos = text.indexOf('_');
        if(pos > -1) {
            _shortcut = text.toLocal8Bit().toLower().at(pos+1);
            shortcuts.insert(1 + (_shortcut - 'a'), this);

            qDebug() << "Registering shortcut" << _shortcut << text;
        } else
            _shortcut = 0;

        blinkTimer.setInterval(150);
        connect(&blinkTimer, SIGNAL(timeout()), this, SLOT(blink()));

        activateTimer.setSingleShot(true);
        connect(&activateTimer, SIGNAL(timeout()), this, SLOT(activate()));

        _activateWait = false;
        _blink = false;
    }

    static bool callShortcut(char s) {
        CursesAction* action = shortcuts.value(s);
        if(action)
            action->mouseClicked(QPoint(0, 0));
        return action;
    }

    char shortcut() {
        return _shortcut;
    }

    QSize sizeForString(QString str) {
        return QSize(2 + str.size(), 1);
    }

    inline void mouseClicked(QPoint) {
        emit clicked();
        emit clickedAt(QPoint(screenX(), screenY()+1));

        focus();
        if(_activateWait)
            return;

        blinkTimer.start();
        activateTimer.start(600);
        _activateWait = true;
    }

protected:
    inline void drawImpl() {
        bool standout = wattr().testFlag(Focused);
        if(_blink)
            standout = !standout;

        int attr = standout ? A_STANDOUT : A_NORMAL;
        if(_activateWait)
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
    }

    inline void textChanged() {markDirty();}

protected slots:
    void blink() {
        _blink = !_blink;
        markDirty();
    }

    void activate() {
        blinkTimer.stop();
        if(_blink || _activateWait) {
            _blink = false;
            _activateWait = false;
            markDirty();
        }
        emit activated();
    }

signals:
    void clickedAt(QPoint);

private:
    bool _blink;
    bool _activateWait;

    char _shortcut;
    QTimer blinkTimer;
    QTimer activateTimer;

    static QHash<char, CursesAction*> shortcuts;
};

#endif // CURSESACTION_H
