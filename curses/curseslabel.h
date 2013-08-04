#ifndef CURSESLABEL_H
#define CURSESLABEL_H

#include <guilabel.h>
#include "cursesbase.h"

class GUIContainer;

class CursesLabel : public GUILabel, public CursesBase
{
    Q_OBJECT
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

    inline Q_INVOKABLE CursesLabel(QString text, GUIContainer* parent =0) : GUILabel(text, parent) {fitToContent();CursesBase::updateParent((CursesBase*)parentContainer());}
    inline Q_INVOKABLE CursesLabel(GUIContainer* parent =0) : GUILabel(parent) {CursesBase::updateParent((CursesBase*)parentContainer());}

    inline void drawImpl() {
        wattrset(hnd(), _attr);
        mvwaddnstr(hnd(), 0, 0, text().toLocal8Bit(), text().toLocal8Bit().size());
    }

    virtual void* internalPtr() {return (void*)thisBasePtr();}
    virtual void* handlePtr() {return (void*)hnd();}

    inline Attr attr() const{return _attr;}
    inline void setAttr(Attr attr) {if(_attr==attr)return;_attr=attr;markDirty();}

    inline void mouseClicked(QPoint) {emit clicked();}
    inline void notifyDirty() {cursesDirtyMainWindow();}
    inline QRect geom() const{return GUIWidget::geom();}

protected:
    inline QSize sizeForString(QString text) {return QSize(text.size(), 1);}

    inline void posChanged() {CursesBase::setPos(pos());}
    inline void sizeChanged() {CursesBase::setSize(size());}
    inline void textChanged() {markDirty();}

    inline void parentChanged() {CursesBase::updateParent((CursesBase*)parentContainer());}

private:
    Attr _attr;
};

#endif // CURSESLABEL_H
