#ifndef CURSESDIALOG_H
#define CURSESDIALOG_H

#include <guidialog.h>

#include "cursesbase.h"

class CursesDialog : public GUIDialog, public CursesWindow
{
    Q_OBJECT
    CURSES_WINDOW
public:
    inline explicit CursesDialog(QString title, GUIContainer *parent) : GUIDialog(title, Padding(QPoint(2, 4), QPoint(2, 2)), parent) {setWAttr(Hidden);}

protected:
    virtual void drawChildren(QRect clip, QPoint off) {
        Padding pad = padding();
        clip &= QRect(pad.first, QPoint(width()-1, height()) - (pad.second+pad.first));

        Children children = childWidgets();
        foreach(GUIWidget* child, children) {
            if(child->isHidden() || child->isWindow())
                continue;

            CursesBase* base = dynamic_cast<CursesBase*>(child);
            if(base)
                drawChild(base, clip, off);
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

    virtual void showImpl() {
        CursesWindow::showImpl();
    }

    virtual void hideImpl() {
        CursesWindow::hideImpl();
    }
};

#endif // CURSESDIALOG_H
