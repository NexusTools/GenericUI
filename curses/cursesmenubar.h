#ifndef CURSESMENUBAR_H
#define CURSESMENUBAR_H

#include <guimenubar.h>

#include "curseslabel.h"

class CursesMenuBar : public GUIMenuBar, public CursesContainer
{
    Q_OBJECT
    CURSES_CONTAINER

public:
    inline CursesMenuBar(GUIContainer* parent) : GUIMenuBar(parent) {}
    inline void addSeparator() {new CursesLabel("|", this);}

    /*virtual void mouseClicked(QPoint p) {
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

protected:
    inline void drawImpl() {
        fixLayoutImpl();
    }

    virtual void drawChildren(QRect clip, QPoint off) {
        Children children = childWidgets();

        qDebug() << children;
        foreach(GUIWidget* child, children) {
            qDebug() << child;

            CursesBase* base = child->internal<CursesBase>();
            if(base)
                drawChild((CursesBase*)base, clip, off);
            qDebug() << child << base;
        }
    }
};

#endif // CURSESMENUBAR_H
