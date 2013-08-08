#ifndef CURSESMENUBAR_H
#define CURSESMENUBAR_H

#include <guimenubar.h>

#include "curseslabel.h"

class CursesAction;

class CursesMenuBar : public GUIMenuBar, public CursesBaseContainer
{
    Q_OBJECT
    CURSES_CONTAINER

    friend class CursesMenu;
public:
    inline CursesMenuBar(GUIContainer* parent) : GUIMenuBar(parent) {}
    inline void addSeparator() {new CursesLabel("|", this);}

    virtual bool processEvent(QEvent *);

protected:
    static bool passShortcut(CursesAction*, Qt::Key);

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
