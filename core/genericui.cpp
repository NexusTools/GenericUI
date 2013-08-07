#include "guimainwindow.h"

#include <QEvent>

int GUIWidget::screenX() const{
    int x = _geom.x();
    GUIContainer* par = parentContainer();
    while(par) {
        x += par->x();
        par = par->parentContainer();
    }
    return x;
}

int GUIWidget::screenY() const{
    int y = _geom.y();
    GUIContainer* par = parentContainer();
    while(par) {
        y += par->y();
        par = par->parentContainer();
    }
    return y;
}

void GUIWidget::setPos(QPoint p) {
    if(p == pos())
        return;

    _geom = QRect(p, size());
    simEvent(GUIEvent::GUISizeChanged);
    simEvent(GUIEvent::GUIGeometryChanged);
}

void GUIWidget::setSize(QSize s) {
    if(s == size())
        return;

    _geom.setSize(s);
    simEvent(GUIEvent::GUISizeChanged);
    simEvent(GUIEvent::GUIGeometryChanged);
}

bool GUIContainer::event(QEvent * ev) {
    switch(ev->type()) {
        case QEvent::ChildAdded:
        case QEvent::ChildRemoved:
            markLayoutDirty();
            break;

        default:
            break;

    }

    return GUIWidget::event(ev);
}

void GUIWidget::setGeom(QRect r) {
    bool s = r.size() == size();
    bool p = r.topLeft() == pos();
    if(s && p)
        return;

    _geom = r;
    if(!p)
        simEvent(GUIEvent::GUIPositionChanged);
    if(!s)
        simEvent(GUIEvent::GUISizeChanged);
    simEvent(GUIEvent::GUIGeometryChanged);
}

void GUIWidget::setDisabled(bool dis) {
    if(wattr().testFlag(Disabled) == dis)
        return;

    if(dis)
        setWAttr(wattr() |= Disabled);
    else
        setWAttr(wattr() ^= Disabled);
    simEvent(GUIEvent::GUIStateChanged);
}

void GUIWidget::setHidden(bool hdn) {
    if(wattr().testFlag(Hidden) == hdn)
        return;

    if(hdn)
        setWAttr(wattr() |= Hidden);
    else
        setWAttr(wattr() ^= Hidden);
    simEvent(GUIEvent::GUIVisibilityChanged);
}
