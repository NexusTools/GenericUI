#ifndef CURSESMENUBAR_H
#define CURSESMENUBAR_H

#include <guimenubar.h>

#include "curseslabel.h"

class CursesMenuBar : public GUIMenuBar, public CursesContainer
{
    Q_OBJECT
    CURSES_CONTAINER(GUIContainer)

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

    inline void drawChildren(QRect clip, QPoint off) {
        foreach(GUIWidget* child, children()) {
            CursesBase* base = dynamic_cast<CursesBase*>(child);
            if(base)
                drawChild(base, clip, off);
        }
    }
};

#endif // CURSESMENUBAR_H
