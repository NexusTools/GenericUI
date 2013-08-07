#ifndef GUICONTAINER_H
#define GUICONTAINER_H

#include "genericui.h"

#include <QPair>
#include <QTimer>
#include <QDebug>

class GUIContainer : public GUIWidget
{
    Q_OBJECT

    friend class GUIWidget;
public:
    typedef QList<GUIWidget*> Children;
    typedef QPair<QPoint,QPoint> Padding;
    typedef QPoint Spacing;

    enum LayoutType {
        FreeformLayout,

        HorizontalLayout,
        VerticalLayout
    };

    virtual QSize preferredSize();
    virtual QSize sizeForLayout();
    Children childWidgets() const;
    virtual void markLayoutDirty() {
        if(wattr().testFlag(DirtyLayout))
            return;

        qDebug() << this << "Marked layout dirty";
        _preferredSize = QSize();
        layoutTimer.start();

        setWAttr(wattr() | DirtyLayout);
        pushEvent(GUIEvent::GUILayoutBecameDirty);
    }

    virtual bool event(QEvent *);

protected:
    explicit GUIContainer(Spacing margin, Padding padding, LayoutType layout =FreeformLayout, GUIContainer *parent =0);
    explicit GUIContainer(Spacing margin, Padding padding, GUIContainer *parent);
    explicit GUIContainer(LayoutType layout, GUIContainer *parent =0);
    explicit GUIContainer(GUIContainer *parent =0);

    virtual void fixLayoutImpl();

protected slots:
    inline void fixLayout() {
        fixLayoutImpl();

        if(wattr().testFlag(DirtyLayout))
            setWAttr(wattr() ^ DirtyLayout);
    }

private:
    LayoutType _layout;
    QSize _preferredSize;

    QTimer layoutTimer;
    Padding _padding;
    Spacing _spacing;
};

#endif // GUICONTAINER_H
