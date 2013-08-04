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
        blinkTimer.setInterval(600);
        connect(&blinkTimer, SIGNAL(timeout()), this, SLOT(blink()));

        activateTimer.setSingleShot(true);
        connect(&activateTimer, SIGNAL(timeout()), this, SLOT(activate()));

        _blink = false;
    }

    QSize sizeForString(QString str) {
        return QSize(2 + str.size(), 1);
    }

    inline void mouseClicked(QPoint) {
        //emit clicked();

        focus();
        blinkTimer.start();
        //activateTimer.start(1200);
    }

protected:
    inline void drawImpl() {
        bool standout = wattr().testFlag(Focused);
        if(_blink)
            standout = !standout;
        wattrset(hnd(), standout ? A_STANDOUT : A_NORMAL);
        wmove(hnd(), 0, 0);
        waddch(hnd(), ' ');
        waddnstr(hnd(), text().toLocal8Bit(), text().toLocal8Bit().size());
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
        if(_blink) {
            _blink = false;
            markDirty();
        }
        emit activated();
    }

private:
    bool _blink;

    QTimer blinkTimer;
    QTimer activateTimer;

};

#endif // CURSESACTION_H
