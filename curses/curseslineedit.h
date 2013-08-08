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
        return QSize(22, sizeForString(text()).height());
    }

protected:
    inline QSize sizeForString(QString text) {return QSize(text.size(), 1);}

    inline void drawImpl() {
        wmove(hnd(), 0, 0);
        wattrset(hnd(), A_STANDOUT);
        //waddch(hnd(), ' '); // ACS_LARROW

        wattron(hnd(), A_UNDERLINE);
        QString visPart = text().mid(0, width());
        waddstr(hnd(), visPart.toLocal8Bit().data());

        if(visPart.length() < width())
            waddstr(hnd(), QByteArray(width()-visPart.length(), ' ').data());

        wattroff(hnd(), A_UNDERLINE);
        //waddch(hnd(), ' '); // ACS_RARROW
        wattrset(hnd(), A_NORMAL);
    }

};

#endif // CURSESLINEEDIT_H
