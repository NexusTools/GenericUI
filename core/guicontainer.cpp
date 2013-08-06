#include "guimainwindow.h"

GUIContainer::GUIContainer(Spacing spacing, Padding padding, LayoutType layout, GUIContainer *parent) : GUIWidget(parent) {
    _padding = padding;
    _spacing = spacing;
    _layout = layout;

    _dirtyLayout = false;
    layoutTimer.setInterval(0);
    layoutTimer.setSingleShot(true);
    connect(&layoutTimer, SIGNAL(timeout()), this, SLOT(fixLayout()));
}

GUIContainer::GUIContainer(Spacing spacing, Padding padding, GUIContainer *parent) : GUIWidget(parent) {
    _layout = FreeformLayout;
    _padding = padding;
    _spacing = spacing;

    _dirtyLayout = false;
    layoutTimer.setInterval(0);
    layoutTimer.setSingleShot(true);
    connect(&layoutTimer, SIGNAL(timeout()), this, SLOT(fixLayout()));
}

GUIContainer::GUIContainer(LayoutType layout, GUIContainer *parent) : GUIWidget(parent) {
    _layout = layout;

    _dirtyLayout = false;
    layoutTimer.setInterval(0);
    layoutTimer.setSingleShot(true);
    connect(&layoutTimer, SIGNAL(timeout()), this, SLOT(fixLayout()));
}

GUIContainer::GUIContainer(GUIContainer *parent) : GUIWidget(parent) {
    _layout = FreeformLayout;

    _dirtyLayout = false;
    layoutTimer.setInterval(0);
    layoutTimer.setSingleShot(true);
    connect(&layoutTimer, SIGNAL(timeout()), this, SLOT(fixLayout()));
}

bool GUIContainer::eventFilter(QObject* obj, QEvent* ev) {
    if(obj->parent() == this && qobject_cast<GUIContainer*>(obj))
        switch(ev->type()) {
            case GUIEvent::GUIGeometryChanged:
            case GUIEvent::GUIWAttrChanged:
            {
                markLayoutDirty();
                break;
            }

            default:
                break;

        }

    return false;
}

GUIContainer* GUIWidget::parentContainer() const{
    return qobject_cast<GUIContainer*>(parent());
}

QSize GUIContainer::sizeForLayout(int maxWidth)  {
    if(maxWidth == -1)
        maxWidth = width();
    else if(maxWidth == 0) {
        GUIContainer* con = parentContainer();
        if(con)
            maxWidth = con->width() - x();
        else
            maxWidth = 80;
    }

    QSize size;
    switch(_layout) {
        case InlineElements:
        {
            int w=0;
            int h=0;
            int lineHeight = 0;
            foreach(GUIWidget* child, childWidgets()) {
                if(child->isHidden())
                    continue;

                QSize pref = child->preferredSize();
                if(pref.width() + w >= maxWidth) {
                    w = 0;
                    h += lineHeight;
                    lineHeight = 0;
                }

                w += pref.width();
                if(pref.height() > lineHeight)
                    lineHeight = pref.height();

                if(w > size.width())
                    size.setWidth(w);
            }
            size.setHeight(h + lineHeight);

            break;
        }

        case BlockElements:
        {
            int h=0;
            foreach(GUIWidget* child, childWidgets()) {
                if(child->isHidden())
                    continue;

                QSize pref = child->preferredSize();
                h += pref.height();

                if(pref.width() > size.width())
                    size.setWidth(pref.width());
            }
            size.setHeight(h);

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
    if(!_dirtyLayout)
        return;
    _dirtyLayout = false;

    switch(_layout) {
        case InlineElements:
        {
            int x=0;
            int y=0;
            int lineHeight = 0;
            foreach(GUIWidget* child, childWidgets()) {
                if(child->width() + x >= width()) {
                    x = 0;
                    y += lineHeight;
                    lineHeight = 0;
                }
                child->move(x, y);

                x += child->width();
                if(child->height() > lineHeight)
                    lineHeight = child->height();
            }

            break;
        }

        case BlockElements:
        {
            int y=0;
            foreach(GUIWidget* child, childWidgets()) {
                if(child->isHidden())
                    continue;

                child->move(0, y);
                y += child->height();
            }

            break;
        }

        case FreeformLayout:
            return;
    }
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
