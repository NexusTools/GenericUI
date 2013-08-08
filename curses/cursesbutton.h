#ifndef CURSESBUTTON_H
#define CURSESBUTTON_H

#include <QTimer>

#include <guibutton.h>

#include "cursesbase.h"

class CursesMenu;

class CursesButton : public GUIButton, public CursesBase
{
    Q_OBJECT
    BASIC_CURSES_OBJECT
public:
    inline CursesButton(QString text, WAttrs attr, GUIContainer* par =0) : GUIButton(text, attr, par) {
        fitToContent();

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

    virtual Qt::Key shortcut() {
        return _shortcut;
    }

    QSize sizeForString(QString str) {
        return QSize((str.contains('_') ? 3 : 4) + str.size(), 1);
    }

protected:
    void drawImpl();
    void feedback();

    virtual bool processEvent(QEvent* e);

public slots:
    void activate();

protected slots:
    void blink() {
        _blink = !_blink;
        markDirty();
    }

    void activateCallback();

signals:
    void selected(QVariant);

private:
    bool _blink;

    Qt::Key _shortcut;
    QTimer blinkTimer;
    QTimer activateTimer;
    bool _activateWait;
    int _spos;
};

#endif // CURSESBUTTON_H
