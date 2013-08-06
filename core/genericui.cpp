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

void GUIWidget::setParent(GUIContainer* par) {
    QObject::setParent(par);
    if(isWindow())
        return;

    if(par) {
        par->_children.append(this);
        //parentChanged();
    }
}

bool GUIWidget::event(QEvent *ev) {
    if(!isWindow())
        switch(ev->type()) {
            case QEvent::ParentAboutToChange:
            {

                GUIContainer* par = parentContainer();
                if(par) {
                    par->_children.removeOne(this);
                    //parentChanged();
                }
                break;
            }

            case QEvent::ParentChange:
            {
                GUIContainer* par = parentContainer();
                if(par) {
                    par->_children.append(this);
                    //parentChanged();
                }
                break;
            }

            default:
                break;

        }

    return QObject::event(ev);
}

void GUIWidget::setPos(QPoint p) {
    if(p == pos())
        return;

    _geom = QRect(p, size());
    pushEvent(GUIEvent::GUISizeChanged);
    pushEvent(GUIEvent::GUIGeometryChanged);
}

void GUIWidget::setSize(QSize s) {
    if(s == size())
        return;

    _geom.setSize(s);
    pushEvent(GUIEvent::GUISizeChanged);
    pushEvent(GUIEvent::GUIGeometryChanged);
}

void GUIWidget::setGeom(QRect r) {
    bool s = r.size() == size();
    bool p = r.topLeft() == pos();
    if(s && p)
        return;

    _geom = r;
    if(!p)
        pushEvent(GUIEvent::GUIPositionChanged);
    if(!s)
        pushEvent(GUIEvent::GUISizeChanged);
    pushEvent(GUIEvent::GUIGeometryChanged);
}

void GUIWidget::setDisabled(bool dis) {
    if(wattr().testFlag(Disabled) == dis)
        return;

    if(dis)
        setWAttr(wattr() |= Disabled);
    else
        setWAttr(wattr() ^= Disabled);
    pushEvent(GUIEvent::GUIStateChanged);
}

void GUIWidget::setHidden(bool hdn) {
    if(wattr().testFlag(Hidden) == hdn)
        return;

    if(hdn)
        setWAttr(wattr() |= Hidden);
    else
        setWAttr(wattr() ^= Hidden);
    pushEvent(GUIEvent::GUIVisibilityChanged);
}

GUIContainer* GUIWidget::parentContainer() const{
    return qobject_cast<GUIContainer*>(parent());
}

template <class T>
bool isCompatible(GUIWidget* widget) {
    return qobject_cast<T*>(widget);
}

GUIMainWindow* GUIWidget::mainWindow() {
    GUIWidget* widget = this;
    while(!isCompatible<GUIMainWindow>(widget))
        widget = widget->parentContainer();

    return (GUIMainWindow*)widget;
}

GUIWindow* GUIWidget::window() {
    GUIWidget* widget = this;
    while(!isCompatible<GUIWidget>(widget))
        widget = widget->parentContainer();

    return (GUIWindow*)widget;
}
