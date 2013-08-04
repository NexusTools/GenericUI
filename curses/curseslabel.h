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

    inline void notifyDirty() {cursesDirtyMainWindow(mainWindow());}
    inline QRect geom() const{return GUIWidget::geom();}

protected:
    inline QSize sizeForString(QString text) {return QSize(text.size(), 1);}


    inline void posChanged() {CursesBase::setPos(pos());}
    inline void sizeChanged() {CursesBase::setSize(size());}
    inline void textChanged() {markDirty();}

    inline void parentChanged() {CursesBase::updateParent((CursesBase*)parentContainer());}

};

#endif // CURSESLABEL_H
