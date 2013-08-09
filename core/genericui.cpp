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
    simEvent(GUIEvent::GUIPositionChanged);
    simEvent(GUIEvent::GUIGeometryChanged);
}

void GUIWidget::setSize(QSize s) {
    if(s == size())
        return;

    _geom.setSize(s);
    simEvent(GUIEvent::GUISizeChanged);
    simEvent(GUIEvent::GUIGeometryChanged);
}

bool GUIWidget::event(QEvent * ev) {
    switch(ev->type()) {
        case GUIEvent::GUIMouseClicked:
            emit clicked();
            break;

        case GUIEvent::GUIVisibilityChanged:
        {
            GUIContainer* con = parentContainer();
            if(con)
                con->markLayoutDirty();
            break;
        }

        default:
            break;

    }

    return QObject::event(ev);
}

bool GUIContainer::event(QEvent * ev) {
    switch(ev->type()) {
        case QEvent::ChildAdded:
        case QEvent::ChildRemoved:
            markLayoutDirty();
            break;

        case GUIEvent::GUIMouseClicked:
        {
            QListIterator<GUIWidget*> i(childWidgets());
            i.toBack();
            while(i.hasPrevious()) {
               GUIWidget* child = i.previous();
               if(child->isWindow() || child->isHidden() || child->isDisabled())
                   continue;

               if(child->geom().contains(((GUIMouseEvent*)ev)->pos())) {
                   ((GUIMouseEvent*)ev)->push(-child->x(), -child->y());
                   child->event(ev);
                   ((GUIMouseEvent*)ev)->pop();
                   return true;
               }
            }

            break;
        }

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
    if(setWAttr(Disabled, dis))
        simEvent(GUIEvent::GUIStateChanged);
}

void GUIWidget::setHidden(bool hdn) {
    if(setWAttr(Hidden, hdn))
        simEvent(GUIEvent::GUIVisibilityChanged);
}

bool GUIWidget::setWAttr(WAttr attr, bool on) {
    if(wattr().testFlag(attr) == on)
        return false;

    if(on)
        return setWAttr(wattr() | attr);
    else
        return setWAttr(wattr() ^ attr);
}
