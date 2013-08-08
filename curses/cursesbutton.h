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

        int pos = text.indexOf('_');
        if(pos > -1)
            _shortcut = (Qt::Key)(text.toLocal8Bit().toLower().at(pos+1) - 'a');
        else
            _shortcut = (Qt::Key)0;

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
};

#endif // CURSESBUTTON_H
