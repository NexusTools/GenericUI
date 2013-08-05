#include "guicontainer.h"
#include "guimainwindow.h"

#include <QEvent>

void GUIWidget::setWAttr(WAttrs attr) {
    _attr = attr;
    GUIContainer* con = parentContainer();
    if(con)
        con->markLayoutDirty();
}

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
        parentChanged();
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
                    parentChanged();
                }
                break;
            }

            case QEvent::ParentChange:
            {
                GUIContainer* par = parentContainer();
                if(par) {
                    par->_children.append(this);
                    parentChanged();
                }
                break;
            }

            default:
                break;

        }

    return QObject::event(ev);
}

void GUIWidget::sizeChanged() {
    GUIContainer* con = parentContainer();
    if(con)
        con->markLayoutDirty();
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
