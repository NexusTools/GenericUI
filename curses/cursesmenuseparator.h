#ifndef CURSESMENUSEPARATOR_H
#define CURSESMENUSEPARATOR_H

#include <guiwindow.h>

#include "cursesbase.h"

class GUIMenu;

class CursesMenuSeparator : public GUIWidget, public CursesBase
{
    Q_OBJECT
    CURSES_OBJECT

public:
    inline CursesMenuSeparator(GUIMenu* menu) : GUIWidget((GUIContainer*)menu) {}

    virtual QSize preferredSize() {
        return QSize(5, 1);
    }

protected:
    inline void drawImpl() {
        wmove(hnd(), 0, 2);
        int left = width()-4;
        while(left > 0) {
            waddch(hnd(), '-');
            left--;
        }
    }

};

#endif // CURSESMENUSEPARATOR_H
