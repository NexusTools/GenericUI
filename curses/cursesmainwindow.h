#ifndef CURSESMAINWINDOW_H
#define CURSESMAINWINDOW_H

#include <guimainwindow.h>
#include <QTimer>

#include "cursesaction.h"

class CursesMainWindow : public GUIMainWindow, public CursesScreen
{
    Q_OBJECT
    CURSES_CONTAINER_CORE

public:
    inline explicit CursesMainWindow(QString title) : GUIMainWindow(title), CursesScreen(init()) {
        _current = this;
        titleChanged();

        init();
        resize(checkSize());

        repaintTimer.setInterval(0);
        repaintTimer.setSingleShot(true);
        connect(&repaintTimer, SIGNAL(timeout()), this, SLOT(drawNow()));
        repaintTimer.start();

        inputTimer.setInterval(30);
        connect(&inputTimer, SIGNAL(timeout()), this, SLOT(readNextCH()));
        inputTimer.start();
    }
    virtual ~CursesMainWindow() {_current=0;}

    static inline CursesMainWindow* current() {return _current;}
    virtual void terminateRequested(int);

    inline void recheckSize() {
        endwin();
        refresh();

        resize(checkSize());
    }

    inline void scheduleRepaint() {
        repaintTimer.start();
    }

    inline void showWindow(CursesWindow* window) {
        if(!_windowStack.contains(window)) {
            _windowStack << window;
            scheduleRepaint();
        }
    }

    inline bool isWindowOpen(CursesWindow* window) {
        return _windowStack.contains(window);
    }

    inline void hideWindow(CursesWindow* window) {
        _windowStack.removeOne(window);
        scheduleRepaint();
    }

    /*virtual void mouseClicked(QPoint p) {
        if(!_windowStack.isEmpty()) {
            CursesWindow* lastWin = _windowStack.last();

            lastWin->mouseClicked(p - lastWin->geom().topLeft());
            return;
        }

        QListIterator<GUIWidget*> i(children());
        i.toBack();
        while(i.hasPrevious()) {
           GUIWidget* child = i.previous();
           CursesBase* curses = child ? child->internal<CursesBase>() : 0;
           if(curses && child->geom().contains(p)) {
               curses->mouseClicked(p - child->geom().topLeft());
               return;
           }
        }


        emit clicked();
    }*/

    virtual bool event(QEvent* ev) {
        bool ret = processEvent(this, ev);
        if(ret)
            return ret;
        return QObject::event(ev);
    }
protected:
    virtual bool eventFilter(QObject* obj, QEvent* ev) {
        bool ret = processEventFilter(obj, ev);
        if(ret)
            return ret;
        return GUIContainer::eventFilter(obj, ev);
    }

    inline virtual void drawImpl() {
        fixLayoutImpl();
    }

    inline void drawChildren(QRect clip, QPoint off) {
        foreach(CursesWindow* child, _windowStack) {
            drawChild(child, clip, off);
        }
        foreach(GUIWidget* child, childWidgets()) {
            CursesBase* base = dynamic_cast<CursesBase*>(child);
            if(base)
                drawChild(base, clip, off);
        }
    }

    inline void titleChanged() {printf("\033]0;%s\007", qPrintable(title()));}
    inline virtual void fixLayoutImpl() {GUIContainer::fixLayoutImpl();markDirty();}

    inline void focusTaken() {setWAttr(wattr() ^ GUIWidget::Focused);markDirty();} \
    inline void focusGiven() {setWAttr(wattr() & GUIWidget::Focused);markDirty();}

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

private:
    static QSize init();
    static CursesMainWindow* _current;

    QList<CursesWindow*> _windowStack;
    QTimer repaintTimer;
    QTimer inputTimer;
};

#endif // CURSESMAINWINDOW_H
