#include "guicontainer.h"
#include "guimainwindow.h"

#include <QEvent>

void GUIWidget::setParent(GUIContainer* par) {
    QObject::setParent(par);
    if(par) {
        par->_children.append(this);
        parentChanged();
    }
}

bool GUIWidget::event(QEvent *ev) {
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

    if(ev->type() == QEvent::ParentChange) {

    }

    return QObject::event(ev);
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
