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
    inline void drawImpl() {
        mvwaddstr(hnd(), 0, 0, "MenuBar Test");
    }
};

#endif // CURSESMENUBAR_H
