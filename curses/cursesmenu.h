#ifndef CURSESMENU_H
#define CURSESMENU_H

#include <guimenu.h>

#include "curseslabel.h"
#include "cursesaction.h"

class CursesMenu : public GUIMenu, public CursesWindow
{
    Q_OBJECT
    CURSES_CONTAINER

public:
    inline CursesMenu(QString title, GUIWindow* parent) : GUIMenu(title, parent) {}


    virtual GUIAction* action(QString name =QString()) {
        if(name.isEmpty())
            name = title();

        CursesAction* action = new CursesAction(name);
        connect(action, SIGNAL(clickedAt(QPoint)), this, SLOT(show(QPoint)));
        return action;
    }

    inline void addAction(GUIAction* action) {action->setParent(this);}
    inline void addMenu(GUIMenu* menu, QString text) {addAction(menu->action(text));}
    inline void addMenu(GUIMenu* menu) {addAction(menu->action());}
    inline void addSeparator() {new CursesLabel(" ------------ ", this);}

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
        wmove(hnd(), 0, 0);
        waddch(hnd(), ACS_ULCORNER);
        int left = width()-2;
        while(left > 0) {
            waddch(hnd(), ACS_HLINE);
            left--;
        }
        waddch(hnd(), ACS_URCORNER);
    }

    inline void drawChildren() {
        foreach(GUIWidget* child, children()) {
            CursesBase* base = dynamic_cast<CursesBase*>(child);
            if(base)
                base->render(this);
        }
    }

    void showImpl();
    void closeImpl();
};

#endif // CURSESMENU_H
