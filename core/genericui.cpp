#include "guicontainer.h"
#include "guimainwindow.h"

#include <QEvent>
#include <QDebug>

void GUIWidget::setParent(GUIContainer* par) {
    QObject::setParent(par);
    if(par) {
        par->_children.append(this);
        parentChanged();
    }
}

bool GUIWidget::event(QEvent *ev) {
    qDebug() << "Event" << ev->type();

    switch(ev->type()) {
        case QEvent::ParentAboutToChange:
        {

            GUIContainer* par = parentContainer();
            qDebug() << parent() << par;
            if(par) {
                par->_children.removeOne(this);
                parentChanged();
            }
            break;
        }

        case QEvent::ParentChange:
        {
            GUIContainer* par = parentContainer();
            qDebug() << parent() << par;
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

GUIMainWindow* GUIWidget::mainWindow() {
    GUIWidget* widget = this;
    while(widget && widget->metaObject() != &GUIMainWindow::staticMetaObject)
        widget = widget->parentContainer();

    return (GUIMainWindow*)widget;
}

GUIWindow* GUIWidget::window() {
    GUIWidget* widget = this;
    while(widget && widget->metaObject() != &GUIWindow::staticMetaObject)
        widget = widget->parentContainer();

    return (GUIWindow*)widget;
}
