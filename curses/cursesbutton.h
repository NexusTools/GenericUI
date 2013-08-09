#ifndef CURSESBUTTON_H
#define CURSESBUTTON_H

#include <QVariant>
#include <QTimer>

#include <guibutton.h>
#include "cursesbase.h"

class CursesButton : public GUIButton, public CursesBase
{
    Q_OBJECT
    BASIC_CURSES_OBJECT
public:
    CursesButton(QString text, WAttrs attr =Normal, GUIContainer* par =0);

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
    bool _default;
    int _spos;
};

#endif // CURSESBUTTON_H
