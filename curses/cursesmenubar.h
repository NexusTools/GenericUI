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

protected:
    virtual void drawChildren(QRect clip, QPoint off) {
        Children children = childWidgets();
        foreach(GUIWidget* child, children) {
            CursesBase* base = child->internal<CursesBase>();
            if(base)
                drawChild((CursesBase*)base, clip, off);
        }
    }
};

#endif // CURSESMENUBAR_H
