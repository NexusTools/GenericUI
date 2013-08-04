#ifndef GUICONTAINER_H
#define GUICONTAINER_H

#include "genericui.h"

#include <QTimer>

class GUIContainer : public GUIWidget
{
    Q_OBJECT

    friend class GUIWidget;
public:
    enum LayoutType {
        FreeformLayout,

        InlineElements,
        BlockElements
    };

    inline QList<GUIWidget*> children() const{return _children;}

protected:
    inline GUIContainer(LayoutType layout, GUIContainer *parent =0) : GUIWidget(parent) {
        _layout = layout;

        layoutTimer.setInterval(0);
        layoutTimer.setSingleShot(true);
        connect(&layoutTimer, SIGNAL(timeout()), this, SLOT(fixLayout()));
    }
    inline GUIContainer(GUIContainer *parent =0) : GUIWidget(parent) {
        layoutTimer.setInterval(0);
        layoutTimer.setSingleShot(true);
        connect(&layoutTimer, SIGNAL(timeout()), this, SLOT(fixLayout()));
    }

    virtual QSize sizeForLayout() {return QSize();}
    inline void sizeChanged() {markLayoutDirty();}

    virtual void fixLayoutImpl() {
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
        layoutTimer.start();
    }

private:
    LayoutType _layout;
    QList<GUIWidget*> _children;

    QTimer layoutTimer;
};

#endif // GUICONTAINER_H
