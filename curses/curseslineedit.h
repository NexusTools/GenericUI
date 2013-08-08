#ifndef CURSESLINEEDIT_H
#define CURSESLINEEDIT_H

#include <guilineedit.h>
#include "cursesbase.h"

class CursesLineEdit : public GUILineEdit, public CursesBase
{
    Q_OBJECT
    CURSES_OBJECT

public:
    inline explicit CursesLineEdit(QString text, GUIContainer* par =0) : GUILineEdit(text, par) {}
    inline explicit CursesLineEdit(GUIContainer* par) : GUILineEdit(par) {}

    virtual QSize preferredSize() {
        return QSize(18, sizeForString(text()).height());
    }

protected:
    inline QSize sizeForString(QString text) {return QSize(text.size(), 1);}

    inline void drawImpl() {
        wmove(hnd(), 0, 0);
        wattrset(hnd(), A_STANDOUT);
        waddch(hnd(), ACS_LARROW);

        wattron(hnd(), A_UNDERLINE);
        QString visPart = text().mid(0, width()-2);
        waddstr(hnd(), visPart.toLocal8Bit().data());

        if(visPart.length() < width()-2)
            waddstr(hnd(), QByteArray(width()-2-visPart.length(), ' ').data());

        wattroff(hnd(), A_UNDERLINE);
        waddch(hnd(), ACS_RARROW);
        wattrset(hnd(), A_NORMAL);
    }

};

#endif // CURSESLINEEDIT_H
