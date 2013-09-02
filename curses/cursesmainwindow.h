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
    explicit CursesMainWindow(QString title);
    virtual ~CursesMainWindow() {_current=0;}

    virtual void repaint() {
        repaintTimer.start();
    }

    static inline CursesMainWindow* current() {return _current;}
    virtual void terminateRequested(int);

    inline void recheckSize() {
        sizeTimer.start();
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

    inline void updateSize() {
        endwin();
        refresh();

        resize(checkSize());
    }

    void focusFirst() {
        GUIWidget* first = nextFocusable(0);
        if(first)
            first->focus();
    }

    void readNextCH();

protected:
    virtual bool processEvent(QEvent *ev);

private:
    static QSize initialScreen();
    static CursesMainWindow* _current;

    QTimer sizeTimer;
    QList<CursesWindow*> _windowStack;
    bool _repaintScheduled;
    QTimer repaintTimer;
    QTimer inputTimer;

};

#endif // CURSESMAINWINDOW_H
