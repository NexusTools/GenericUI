#ifndef CURSESMAINWINDOW_H
#define CURSESMAINWINDOW_H

#include <guimainwindow.h>
#include <QTimer>

#include "cursesbase.h"

class CursesMainWindow : public GUIMainWindow, public CursesScreen
{
    Q_OBJECT
    CURSES_CONTAINER_CORE

public:
    inline explicit CursesMainWindow(QString title) : GUIMainWindow(title), CursesScreen() {
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

    inline void recheckSize() {
        endwin();
        refresh();

        resize(checkSize());
    }

    inline void notifyDirty() {
        repaintTimer.start();
    }

    inline void parentChanged() {CursesBase::updateParent((CursesBase*)parentContainer());}

    virtual void mouseClicked(QPoint p) {
        GUIChildren::Iterator i = children().end();
        while(i != children().begin()) {
            i--;
            if((*i)->geom().contains(p)) {
                (*i)->internal<CursesBase>()->mouseClicked(p - (*i)->geom().topLeft());
                return;
            }
        }

        emit clicked();
    }

protected:
    inline virtual void drawImpl() {
        fixLayoutImpl();
    }

    inline void drawChildren() {
        foreach(GUIWidget* child, children()) {
            CursesBase* base = dynamic_cast<CursesBase*>(child);
            if(base)
                base->render(this);
        }
    }

    inline void titleChanged() {printf("\033]0;%s\007", qPrintable(title()));}
    inline virtual void fixLayoutImpl() {GUIContainer::fixLayoutImpl();markDirty();}

    inline void focusTaken() {setWAttr(wattr() ^ GUIWidget::Focused);markDirty();} \
    inline void focusGiven() {setWAttr(wattr() & GUIWidget::Focused);markDirty();}

protected slots:
    inline void drawNow() {
        render(0);
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
                    if(ch > 0 && ch < KEY_MAX) {
                        //TODO: Process normal keys
                    } else
                        qWarning() << "Unknown key" << ch;
                    break;
            }

            ch = getch();
        }
    }

private:
    static void init();
    static CursesMainWindow* _current;

    QTimer repaintTimer;
    QTimer inputTimer;
};

#endif // CURSESMAINWINDOW_H
