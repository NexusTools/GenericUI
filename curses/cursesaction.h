#ifndef CURSESACTION_H
#define CURSESACTION_H

#include <QTimer>

#include <guiaction.h>

#include "cursesbase.h"

class CursesMenu;

class CursesAction : public GUIAction, public CursesBase
{
    Q_OBJECT
    BASIC_CURSES_OBJECT

    friend class CursesMenu;
    friend class CursesMenuBar;
public:
    inline CursesAction(QString text, GUIContainer* par =0) : GUIAction(text, GUIWidget::ExpandWidth, par) {
        fitToContent();

        _spos = text.indexOf('_');
        if(_spos > -1) {
            int shortcut = text.toLocal8Bit().toLower().at(_spos+1) - 'a';
            shortcuts.insert(1 + shortcut, this);
            _shortcut = (Qt::Key)(Qt::Key_A + shortcut);

            qDebug() << "Registering shortcut" << _shortcut << text;
        } else
            _shortcut = (Qt::Key)0;
        _spos++;

        _menu = 0;
        blinkTimer.setInterval(100);
        connect(&blinkTimer, SIGNAL(timeout()), this, SLOT(blink()));

        activateTimer.setSingleShot(true);
        connect(&activateTimer, SIGNAL(timeout()), this, SLOT(activateCallback()));

        _activateWait = false;
        _blink = false;
    }

    static bool callShortcut(char s) {
        CursesAction* action = shortcuts.value(s);
        if(action)
            action->activate();
        return action;
    }

    virtual Qt::Key shortcut() {
        return _shortcut;
    }

    QSize sizeForString(QString str) {
        return QSize((str.contains('_') ? 1 : 2) + str.size(), 1);
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

private:
    bool _blink;
    bool _activateWait;

    int _spos;
    Qt::Key _shortcut;
    QTimer blinkTimer;
    QTimer activateTimer;
    CursesMenu* _menu;

    static QHash<char, CursesAction*> shortcuts;
};

#endif // CURSESACTION_H
