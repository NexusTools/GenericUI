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
                break;

            case BlockElements:
                break;

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
