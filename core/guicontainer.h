#ifndef GUICONTAINER_H
#define GUICONTAINER_H

#include "genericui.h"

#include <QTimer>

class GUIContainer : public GUIWidget
{
    Q_OBJECT

    friend class GUIWidget;
public:
    typedef QList<GUIWidget*> GUIChildren;

    enum LayoutType {
        FreeformLayout,

        InlineElements,
        BlockElements
    };

    virtual QSize sizeForLayout(int maxWidth =0) {
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

    virtual QSize preferredSize() {return sizeForLayout();}
    inline GUIChildren children() const{return _children;}

protected:
    inline GUIContainer(LayoutType layout, GUIContainer *parent =0) : GUIWidget(parent) {
        _layout = layout;

        _dirtyLayout = false;
        layoutTimer.setInterval(0);
        layoutTimer.setSingleShot(true);
        connect(&layoutTimer, SIGNAL(timeout()), this, SLOT(fixLayout()));
    }
    inline GUIContainer(GUIContainer *parent =0) : GUIWidget(parent) {
        _dirtyLayout = false;
        layoutTimer.setInterval(0);
        layoutTimer.setSingleShot(true);
        connect(&layoutTimer, SIGNAL(timeout()), this, SLOT(fixLayout()));
    }
    inline void sizeChanged() {GUIWidget::sizeChanged();markLayoutDirty();}
    virtual void layoutBecameDirty() {}
    virtual void fixLayoutImpl() {
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
                    child->move(0, y);
                    y += child->height();
                }

                break;
            }

            case FreeformLayout:
                return;
        }
    }

protected slots:
    inline void fixLayout() {
        fixLayoutImpl();
    }
    inline void markLayoutDirty() {
        layoutBecameDirty();
        if(_dirtyLayout)
            return;

        layoutTimer.start();
        _dirtyLayout = true;
    }

private:
    bool _dirtyLayout;
    LayoutType _layout;
    GUIChildren _children;

    QTimer layoutTimer;
};

#endif // GUICONTAINER_H
