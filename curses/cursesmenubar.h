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
        foreach(GUIWidget* widget, children()) {
            if(widget->geom().contains(p)) {
                widget->internal<CursesBase>()->mouseClicked(p - widget->geom().topLeft());
                return;
            }
        }

        emit clicked();
    }

protected:
    inline void drawImpl() {
        fixLayoutImpl();
    }

    inline void drawChildren() {
        foreach(GUIWidget* child, children()) {
            CursesBase* base = dynamic_cast<CursesBase*>(child);
            if(base)
                base->render(this);
        }
    }
};

#endif // CURSESMENUBAR_H
