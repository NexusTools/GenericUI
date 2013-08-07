#include "guimainwindow.h"

GUIContainer::GUIContainer(Spacing spacing, Padding padding, LayoutType layout, GUIContainer *parent) : GUIWidget(parent) {
    init(spacing, padding, layout);
}

GUIContainer::GUIContainer(Spacing spacing, Padding padding, GUIContainer *parent) : GUIWidget(parent) {
    init(spacing, padding);
}

GUIContainer::GUIContainer(Spacing spacing, LayoutType layout, GUIContainer *parent) : GUIWidget(parent) {
    init(spacing, layout);
}

GUIContainer::GUIContainer(Spacing spacing, GUIContainer *parent) : GUIWidget(parent) {
    init(spacing);
}

GUIContainer::GUIContainer(Padding padding, LayoutType layout, GUIContainer *parent) : GUIWidget(parent) {
    init(padding, layout);
}

GUIContainer::GUIContainer(Padding padding, GUIContainer *parent) : GUIWidget(parent) {
    init(padding);
}

GUIContainer::GUIContainer(LayoutType layout, GUIContainer *parent) : GUIWidget(parent) {
    init(layout);
}

GUIContainer::GUIContainer(GUIContainer *parent) : GUIWidget(parent) {
    init();
}

void GUIContainer::init(Spacing spacing, Padding padding, LayoutType layout) {
    _padding = padding;
    _spacing = spacing;
    _layout = layout;

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
    Children chilen;
#ifdef LEGACY_QT
    foreach(QObject* chd, children())
        if(qobject_cast<GUIWidget*>(chd))
            chilen << (GUIWidget*)chd;
#else
    chilen = findChildren<GUIWidget*>("", Qt::FindDirectChildrenOnly);
#endif

    return chilen;
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

    return size + QSize(_padding.first.x()+_padding.second.x(),
                         _padding.first.y()+_padding.second.y());
}

void GUIContainer::fixLayoutImpl() {
    QSize prefSize = preferredSize();
    switch(_layout) {
        case HorizontalLayout:
        {
            int x=_padding.first.x();
            int size = prefSize.height()-_padding.first.y()-_padding.second.y();
            foreach(GUIWidget* child, childWidgets()) {
                if(child->isHidden())
                    continue;

                child->setGeom(QRect(QPoint(x, _padding.first.y()),
                         QSize(child->preferredSize().width(), size)));
                x += child->width();
            }

            break;
        }

        case VerticalLayout:
        {
            int y=_padding.first.y();
            int size = prefSize.width()-_padding.first.x()-_padding.second.x();
            foreach(GUIWidget* child, childWidgets()) {
                if(child->isHidden())
                    continue;

                child->setGeom(QRect(QPoint(_padding.first.x(), y),
                         QSize(size, child->preferredSize().height())));
                y += child->height();
            }

            break;
        }

        case FreeformLayout:
            return;
    }

    setSize(prefSize);
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
