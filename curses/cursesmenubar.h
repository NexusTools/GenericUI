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
    inline void drawImpl() {
        fixLayoutImpl();
    }

    inline void drawChildren(WINDOW* buffer, QRect clip, QPoint off) {
        foreach(GUIWidget* child, children()) {
            CursesBase* base = dynamic_cast<CursesBase*>(child);
            if(base)
                drawChild(base, buffer, clip, off);
        }
    }
};

#endif // CURSESMENUBAR_H
