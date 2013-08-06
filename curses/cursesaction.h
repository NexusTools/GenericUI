#ifndef CURSESACTION_H
#define CURSESACTION_H

#include <QTimer>

#include <guiaction.h>

#include "cursesbase.h"

class CursesMenu;

class CursesAction : public GUIAction, public CursesBase
{
    Q_OBJECT
    BASIC_CURSES_OBJECT(QObject)

    friend class CursesMenu;
public:
    inline CursesAction(QString text, GUIContainer* par =0) : GUIAction(text, par) {
        fitToContent();

        int pos = text.indexOf('_');
        if(pos > -1) {
            _shortcut = text.toLocal8Bit().toLower().at(pos+1);
            shortcuts.insert(1 + (_shortcut - 'a'), this);

            qDebug() << "Registering shortcut" << _shortcut << text;
        } else
            _shortcut = 0;

        _menu = 0;
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
        return QSize((str.contains('_') ? 1 : 2) + str.size(), 1);
    }

    inline void mouseClicked(QPoint) {
        clickFeedback();
        emit clicked();
        //focus();

        if(_activateWait)
            return;

        _activateWait = true;
    }

protected:
    void drawImpl();
    void clickFeedback();

    virtual void textChanged() {markDirty();}
    virtual void stateChanged() {markDirty();}

protected slots:
    void blink() {
        _blink = !_blink;
        markDirty();
    }

    void activate();

private:
    bool _blink;
    bool _activateWait;

    char _shortcut;
    QTimer blinkTimer;
    QTimer activateTimer;
    CursesMenu* _menu;

    static QHash<char, CursesAction*> shortcuts;
};

#endif // CURSESACTION_H
