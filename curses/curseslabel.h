#ifndef CURSESLABEL_H
#define CURSESLABEL_H

#include <guilabel.h>
#include "cursesbase.h"

class GUIContainer;

class CursesLabel : public GUILabel, public CursesBase
{
    Q_OBJECT
    CURSES_OBJECT

public:
    enum AttrFlag {
        Normal = A_NORMAL,
        Standout = A_STANDOUT,
        Underline = A_UNDERLINE,
        Reverse = A_REVERSE,
        Dim = A_DIM,
        Bold = A_BOLD
    };
    Q_DECLARE_FLAGS(Attr, AttrFlag)

    Q_INVOKABLE inline CursesLabel(QString text, GUIContainer* parent =0) : GUILabel(text, parent) {fitToContent();}
    Q_INVOKABLE inline CursesLabel(GUIContainer* parent =0) : GUILabel(parent) {}

    inline Attr attr() const{return _attr;}
    inline void setAttr(Attr attr) {if(_attr==attr)return;_attr=attr;markDirty();}

protected:
    inline QSize sizeForString(QString text) {return QSize(text.size(), 1);}

    inline void drawImpl() {
        wattrset(hnd(), _attr);
        mvwaddnstr(hnd(), 0, 0, text().toLocal8Bit(), text().toLocal8Bit().size());
    }

private:
    Attr _attr;
};

#endif // CURSESLABEL_H
