#ifndef CURSESMENU_H
#define CURSESMENU_H

#include <QPointer>

#include <guimenu.h>

#include "curseslabel.h"
#include "cursesaction.h"
#include "cursesmenuseparator.h"

class CursesMenu : public GUIMenu, public CursesWindow
{
    Q_OBJECT
    CURSES_CONTAINER

    friend class CursesAction;
public:
    inline CursesMenu(QString title, GUIWindow* parent) : GUIMenu(title, parent) {_action=0;}


    virtual GUIAction* action(QString name =QString()) {
        if(name.isEmpty())
            name = title();

        _action = new CursesAction(name);
        _action->_menu = this;
        return _action.data();
    }

    inline void addAction(GUIAction* action) {action->setParent(this);}
    inline void addMenu(GUIMenu* menu, QString text) {addAction(menu->action(text));}
    inline void addMenu(GUIMenu* menu) {addAction(menu->action());}
    inline void addSeparator() {new CursesMenuSeparator(this);}

    virtual void mouseClicked(QPoint p) {
        GUIChildren::Iterator i = children().end();
        while(i != children().begin()) {
            i--;
            if((*i)->geom().contains(p)) {
                (*i)->internal<CursesBase>()->mouseClicked(p - (*i)->geom().topLeft());
                return;
            }
        }

        close();
    }

    inline QSize sizeForLayout(int) {
        int h = 0;
        int w = 0;
        foreach(GUIWidget* child, children()) {
            h+=child->height();
            if(child->width() > w)
                w = child->width();
        }

        return QSize(2+w,2+h);
    }

    bool isOpen();

protected:
    inline void fixLayoutImpl() {
        int y = 1;
        foreach(GUIWidget* child, children()) {
            child->move(1, y);
            child->resize(width()-2, 1);
            y+=child->height();
        }
    }

    inline virtual void drawImpl() {
        wmove(hnd(), 0, 0);
        waddch(hnd(), ACS_ULCORNER);
        int left = width()-2;
        while(left > 0) {
            waddch(hnd(), ACS_HLINE);
            left--;
        }
        waddch(hnd(), ACS_URCORNER);

        for(int y=1; y<height()-1; y++) {
            mvwaddch(hnd(), y, 0, ACS_VLINE);
            mvwaddch(hnd(), y, width()-1, ACS_VLINE);
        }

        wmove(hnd(), height()-1, 0);
        waddch(hnd(), ACS_LLCORNER);
        left = width()-2;
        while(left > 0) {
            waddch(hnd(), ACS_HLINE);
            left--;
        }
        waddch(hnd(), ACS_LRCORNER);
    }

    inline void drawChildren(WINDOW* buffer, QRect clip, QPoint off) {
        foreach(GUIWidget* child, children()) {
            CursesBase* base = dynamic_cast<CursesBase*>(child);
            if(base)
                drawChild(base, buffer, clip, off);
        }
    }

    void showImpl();
    void closeImpl();
    void showChain();

private:
    QPointer<CursesAction> _action;
};

#endif // CURSESMENU_H
