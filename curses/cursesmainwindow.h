#ifndef CURSESMAINWINDOW_H
#define CURSESMAINWINDOW_H

#include <guimainwindow.h>
#include <QTimer>

#include "cursesaction.h"

class CursesMainWindow : public GUIMainWindow, public CursesScreen
{
    Q_OBJECT
    CURSES_WINDOW

public:
    inline explicit CursesMainWindow(QString title) : GUIMainWindow(title), CursesScreen(initialScreen()) {
        _current = this;
        resize(initialScreen());
        titleChanged();

        repaintTimer.setInterval(0);
        repaintTimer.setSingleShot(true);
        connect(&repaintTimer, SIGNAL(timeout()), this, SLOT(drawNow()));

        inputTimer.setInterval(50);
        connect(&inputTimer, SIGNAL(timeout()), this, SLOT(readNextCH()));
        inputTimer.start();

        setWAttr(NoAutoResize);
    }
    virtual ~CursesMainWindow() {_current=0;}

    virtual void repaint() {
        repaintTimer.start();
    }

    static inline CursesMainWindow* current() {return _current;}
    virtual void terminateRequested(int);

    inline void recheckSize() {
        endwin();
        refresh();

        resize(checkSize());
    }

    inline void showWindow(CursesWindow* window) {
        if(!_windowStack.contains(window)) {
            _windowStack << window;
            repaint();
        }
    }

    inline bool isWindowOpen(CursesWindow* window) {
        return _windowStack.contains(window);
    }

    inline void hideWindow(CursesWindow* window) {
        _windowStack.removeOne(window);
        repaint();
    }

    virtual void drawChildren(QRect clip, QPoint off) {
        Children children = childWidgets();
        foreach(GUIWidget* child, children) {
            if(child->isHidden() || child->isWindow())
                continue;

            CursesBase* base = dynamic_cast<CursesBase*>(child);
            if(base)
                drawChild(base, clip, off);
        }

        foreach(CursesWindow* child, _windowStack)
            drawChild(child, clip, off);
    }

    inline void titleChanged() {printf("\033]0;%s\007", qPrintable(title()));}

protected slots:
    inline void drawNow() {
        render();
    }

    void readNextCH();

protected:
    virtual bool processEvent(QEvent *ev);

private:
    static QSize initialScreen();
    static CursesMainWindow* _current;

    QList<CursesWindow*> _windowStack;
    bool _repaintScheduled;
    QTimer repaintTimer;
    QTimer inputTimer;

};

#endif // CURSESMAINWINDOW_H
