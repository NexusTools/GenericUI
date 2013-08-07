#include "guimainwindow.h"

GUIContainer::GUIContainer(Spacing spacing, Padding padding, LayoutType layout, GUIContainer *parent) : GUIWidget(parent) {
    _padding = padding;
    _spacing = spacing;
    _layout = layout;

    layoutTimer.setInterval(0);
    layoutTimer.setSingleShot(true);
    connect(&layoutTimer, SIGNAL(timeout()), this, SLOT(fixLayout()));
}

GUIContainer::GUIContainer(Spacing spacing, Padding padding, GUIContainer *parent) : GUIWidget(parent) {
    _layout = FreeformLayout;
    _padding = padding;
    _spacing = spacing;

    layoutTimer.setInterval(0);
    layoutTimer.setSingleShot(true);
    connect(&layoutTimer, SIGNAL(timeout()), this, SLOT(fixLayout()));
}

GUIContainer::GUIContainer(LayoutType layout, GUIContainer *parent) : GUIWidget(parent) {
    _layout = layout;

    layoutTimer.setInterval(0);
    layoutTimer.setSingleShot(true);
    connect(&layoutTimer, SIGNAL(timeout()), this, SLOT(fixLayout()));
}

GUIContainer::GUIContainer(GUIContainer *parent) : GUIWidget(parent) {
    _layout = FreeformLayout;

    layoutTimer.setInterval(0);
    layoutTimer.setSingleShot(true);
    connect(&layoutTimer, SIGNAL(timeout()), this, SLOT(fixLayout()));
}

QSize GUIContainer::preferredSize() {
    if(_preferredSize.width() < 1 || _preferredSize.height() < 1)
        _preferredSize = sizeForLayout();

    return _preferredSize;
}

GUIContainer::Children GUIContainer::childWidgets() const{
    Children children = findChildren<GUIWidget*>("", Qt::FindDirectChildrenOnly);

    return children;
}

GUIContainer* GUIWidget::parentContainer() const{
    return qobject_cast<GUIContainer*>(parent());
}

QSize GUIContainer::sizeForLayout()  {
    QSize size(0, 0);
    switch(_layout) {
        case HorizontalLayout:
        {
            foreach(GUIWidget* child, childWidgets()) {
                if(child->isHidden())
                    continue;

                QSize pref = child->preferredSize();
                size.setWidth(size.width() + pref.width());
                if(pref.height() > size.height())
                    size.setHeight(pref.height());
            }

            break;
        }

        case VerticalLayout:
        {
            foreach(GUIWidget* child, childWidgets()) {
                if(child->isHidden())
                    continue;

                QSize pref = child->preferredSize();
                size.setHeight(size.height() + pref.height());
                if(pref.width() > size.width())
                    size.setWidth(pref.width());
            }

            break;
        }

        case FreeformLayout:
            foreach(GUIWidget* child, childWidgets()) {
                int w = child->x() + child->width();
                int h = child->y() + child->height();

                if(w > size.width())
                    size.setWidth(w);
                if(h > size.height())
                    size.setHeight(h);
            }

    }

    return size;
}

void GUIContainer::fixLayoutImpl() {
    if(!wattr().testFlag(DirtyLayout))
        return;

    switch(_layout) {
        case HorizontalLayout:
        {
            int x=0;
            foreach(GUIWidget* child, childWidgets()) {
                if(child->isHidden())
                    continue;

                child->setGeom(QRect(QPoint(x, 0), child->preferredSize()));
                x += child->width();
            }

            break;
        }

        case VerticalLayout:
        {
            int y=0;
            foreach(GUIWidget* child, childWidgets()) {
                if(child->isHidden())
                    continue;

                child->setGeom(QRect(QPoint(0, y), child->preferredSize()));
                y += child->height();
            }

            break;
        }

        case FreeformLayout:
            return;
    }

    setSize(preferredSize());
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

GUIMainWindow::GUIMainWindow(QString title) : GUIWindow(title, 0)
{
    qRegisterMetaType<GUIEvent::GUIType>("GUIEvent::GUIType");
}
