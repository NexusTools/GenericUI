#ifndef CURSESLABEL_H
#define CURSESLABEL_H

#include <guilabel.h>
#include "cursesbase.h"

class GUIContainer;

class CursesLabel : public GUILabel, public CursesBase
{
    Q_OBJECT
public:
    inline Q_INVOKABLE CursesLabel(QString text ="", GUIContainer* parent =0) : GUILabel(text, parent) {fitToContent();CursesBase::updateParent((CursesBase*)parentContainer());}

    inline void drawImpl() {
        mvwaddnstr(hnd(), 0, 0, text().toLocal8Bit(), text().toLocal8Bit().size());
    }

    inline void notifyDirty() {
        CursesBase* par = (CursesBase*)parentContainer();
        if(par)
            par->notifyDirty();
    }
    inline QRect geom() const{return GUIWidget::geom();}

    inline void posChanged(QPoint p) {CursesBase::setPos(p);}
    inline void sizeChanged(QSize s) {CursesBase::setSize(s);}

    inline void parentChanged() {CursesBase::updateParent((CursesBase*)parentContainer());}

protected:
    inline QSize sizeForString(QString text) {return QSize(text.size(), 1);}
};

#endif // CURSESLABEL_H
