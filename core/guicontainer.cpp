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

    layoutTimer.setInterval(0);
    layoutTimer.setSingleShot(true);
    connect(&layoutTimer, SIGNAL(timeout()), this, SLOT(fixLayout()));
    setLayout(layout);
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

int valForFloatAttr(GUIWidget::WAttrs wattr) {
    if(wattr.testFlag(GUIWidget::FloatCenter))
        return 1;
    if(wattr.testFlag(GUIWidget::FloatRight))
        return 2;
    return 0;
}

QSize GUIContainer::sizeForLayout()  {
    QSize size(0, 0);
    switch(_layout) {
        case HorizontalLayout:
        {
            bool first[3] = {true, true, true};

            foreach(GUIWidget* child, childWidgets()) {
                if(child->isHidden())
                    continue;

                QSize pref = child->preferredSize();
                int flot = valForFloatAttr(child->wattr());
                if(first[flot])
                    first[flot] = false;
                else
                    pref.setWidth(pref.width() + spacing().x());

                size.setWidth(size.width() + pref.width());
                if(pref.height() > size.height())
                    size.setHeight(pref.height());
            }

            break;
        }

        case VerticalLayout:
        {
            bool first[3] = {true, true, true};

            foreach(GUIWidget* child, childWidgets()) {
                if(child->isHidden())
                    continue;

                QSize pref = child->preferredSize();
                int flot = valForFloatAttr(child->wattr());
                if(first[flot])
                    first[flot] = false;
                else
                    pref.setHeight(pref.height() + spacing().y());

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

    return size += QSize(_padding.first.x()+_padding.second.x(),
                         _padding.first.y()+_padding.second.y());
}

void GUIContainer::fixLayoutImpl() {
    QSize prefSize = preferredSize();
    if(wattr().testFlag(ExpandWidth) ||
            wattr().testFlag(ExpandHeight)) {
        GUIContainer* con = parentContainer();
        if(con) {
            if(wattr().testFlag(ExpandWidth) &&
                    con->layout() != HorizontalLayout)
                prefSize.setWidth(width());
            if(wattr().testFlag(ExpandHeight) &&
                    con->layout() != VerticalLayout)
                prefSize.setHeight(height());
        }
    }


    switch(_layout) {
        case HorizontalLayout:
        {
            int centerWidth = 0;
            Children centerChilds;
            int x=_padding.first.x();
            int xr=_padding.second.x();
            int size = prefSize.height()-_padding.first.y()-_padding.second.y();
            foreach(GUIWidget* child, childWidgets()) {
                if(child->isHidden())
                    continue;

                QSize pref = child->preferredSize();
                if(child->wattr().testFlag(FloatCenter)) {
                    if(centerWidth > 0)
                        centerWidth += spacing().x();

                    centerWidth += pref.width();
                    centerChilds << child;
                    continue;
                }

                if(child->wattr().testFlag(FloatRight)) {
                    xr += pref.width();
                    child->setGeom(QRect(QPoint(width()-xr, _padding.first.y()),
                             QSize(pref.width(),
                             child->wattr().testFlag(GUIWidget::ExpandHeight) ? size : pref.height())));
                    xr += spacing().x();
                } else {
                    child->setGeom(QRect(QPoint(x, _padding.first.y()),
                             QSize(pref.width(),
                             child->wattr().testFlag(GUIWidget::ExpandHeight) ? size : pref.height())));
                    x += pref.width() + spacing().x();
                }
            }

            x = prefSize.width()/2 - centerWidth/2;
            foreach(GUIWidget* child, centerChilds) {
                QSize pref = child->preferredSize();
                child->setGeom(QRect(QPoint(x, _padding.first.y()), pref));
                x += pref.width() + spacing().x();
            }

            break;
        }

        case VerticalLayout:
        {
            int centerHeight = 0;
            Children centerChilds;
            int y=_padding.first.y();
            int yr=_padding.second.y();
            int size = prefSize.width()-_padding.first.x()-_padding.second.x();
            foreach(GUIWidget* child, childWidgets()) {
                if(child->isHidden())
                    continue;

                QSize pref = child->preferredSize();
                if(child->wattr().testFlag(FloatCenter)) {
                    if(centerHeight > 0)
                        centerHeight += spacing().y();
                    centerHeight += pref.height();
                    centerChilds << child;
                    continue;
                }

                if(child->wattr().testFlag(FloatRight)) {
                    yr += pref.height();
                    child->setGeom(QRect(QPoint(_padding.first.x(), height()-yr),
                             QSize(child->wattr().testFlag(GUIWidget::ExpandWidth) ? size : pref.width(), pref.height())));
                    yr += spacing().y();
                } else {
                    child->setGeom(QRect(QPoint(_padding.first.x(), y),
                             QSize(child->wattr().testFlag(GUIWidget::ExpandWidth) ? size : pref.width(), pref.height())));
                    y += pref.height() + spacing().y();
                }
            }

            y = prefSize.height()/2 - centerHeight/2;
            foreach(GUIWidget* child, centerChilds) {
                QSize pref = child->preferredSize();
                child->setGeom(QRect(QPoint(_padding.first.x(), y), pref));
                y += pref.height() + spacing().y();
            }

            break;
        }

        default:
            break;
    }

    if(!wattr().testFlag(NoAutoResize) &&
            !wattr().testFlag(ExpandWidth) &&
            !wattr().testFlag(ExpandHeight))
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
    while(!isCompatible<GUIWindow>(widget))
        widget = widget->parentContainer();

    return (GUIWindow*)widget;
}

GUIMainWindow::GUIMainWindow(QString title) : GUIWindow(title, 0)
{
//    qRegisterMetaType<GUIEvent::GUIType>("GUIEvent::GUIType");
}
