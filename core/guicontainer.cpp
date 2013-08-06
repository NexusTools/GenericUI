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
    if(ev->type() == QEvent::Destroy) {
        Children::Iterator i = _children.begin();
        while(i != _children.end()) {
            if(((QObject*)*i) == obj) {
                _children.erase(i);
                break;
            }
            i++;
        }
    } else {
        GUIWidget* child = qobject_cast<GUIContainer*>(obj);
        if(child) {
            switch(ev->type()) {
                case GUIEvent::GUIGeometryChanged:
                case GUIEvent::GUIWAttrChanged:
                    markLayoutDirty();
                    break;

                default:
                    break;

            }
        }
    }

    return false;
}

bool GUIContainer::event(QEvent *ev) {
    switch(ev->type()) {
        case QEvent::ChildAdded:
        {
            QChildEvent* cEv = dynamic_cast<QChildEvent*>(ev);
            if(cEv) {
                GUIWidget* child = qobject_cast<GUIContainer*>(cEv->child());
                if(child) {
                    _children.append(child);
                    child->installEventFilter(this);
                }
            }

            break;
        }

        case QEvent::ChildRemoved:
        {
            QChildEvent* cEv = dynamic_cast<QChildEvent*>(ev);
            if(cEv) {
                GUIWidget* child = qobject_cast<GUIContainer*>(cEv->child());
                if(child) {
                    child->removeEventFilter(this);
                    _children.removeOne(child);
                }
            }

            break;
        }

        default:
            break;

    }

    return QObject::event(ev);
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
            foreach(GUIWidget* child, children()) {
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
            foreach(GUIWidget* child, children()) {
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
            foreach(GUIWidget* child, children()) {
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
            foreach(GUIWidget* child, children()) {
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
            foreach(GUIWidget* child, children()) {
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
