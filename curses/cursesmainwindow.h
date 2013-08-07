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
    inline explicit CursesMainWindow(QString title) : GUIMainWindow(title), CursesScreen(init()) {
        _current = this;
        titleChanged();

        init();
        resize(checkSize());

        repaintTimer.setInterval(20);
        repaintTimer.setSingleShot(true);
        connect(&repaintTimer, SIGNAL(timeout()), this, SLOT(drawNow()));

        inputTimer.setInterval(20);
        connect(&inputTimer, SIGNAL(timeout()), this, SLOT(readNextCH()));
        inputTimer.start();
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
        foreach(CursesWindow* child, _windowStack) {
            drawChild(child, clip, off);
        }

        Children children = childWidgets();
        foreach(GUIWidget* child, children) {
            if(child->isHidden() || child->isWindow())
                continue;

            CursesBase* base = dynamic_cast<CursesBase*>(child);
            if(base)
                drawChild(base, clip, off);
        }
    }

    inline void titleChanged() {printf("\033]0;%s\007", qPrintable(title()));}

protected slots:
    inline void drawNow() {
        render();
    }

    inline void readNextCH() {
        MEVENT mEvent;

        int ch = getch();
        while(ch != ERR) {
            switch(ch) {
                case KEY_MOUSE:
                    if(getmouse(&mEvent) == OK)
                        processMouseEvent(mEvent);
                    break;

                case KEY_RESIZE:
                    resize(checkSize());
                    break;

                default:
                    if(!_windowStack.isEmpty()) {
                        //CursesWindow* lastWin = _windowStack.last();

                        // Give key to window for processing
                        return;
                    }


                    if(ch < 27)
                        CursesAction::callShortcut(ch);
                    else if(ch > 0 && ch < KEY_MAX) {
                        qWarning() << "Unknown key" << ch;
                    } else
                        qWarning() << "Unknown key" << ch;
                    break;
            }

            ch = getch();
        }
    }

protected:
    virtual bool processEvent(QEvent *ev);

private:
    static QSize init();
    static CursesMainWindow* _current;

    QList<CursesWindow*> _windowStack;
    bool _repaintScheduled;
    QTimer repaintTimer;
    QTimer inputTimer;

};

#endif // CURSESMAINWINDOW_H
