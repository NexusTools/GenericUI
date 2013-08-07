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
        simEvent(GUIEvent::GUILayoutBecameDirty);
    }

    inline void setLayout(LayoutType l) {
        if(l != FreeformLayout)
            markLayoutDirty();
        _layout = l;
    }

    inline LayoutType layout() const{return _layout;}

    inline Spacing spacing() const{return _spacing;}
    inline Padding padding() const{return _padding;}

    virtual bool event(QEvent *);

protected:
    explicit GUIContainer(Spacing margin, Padding padding =Padding(), LayoutType layout =FreeformLayout, GUIContainer *parent =0);
    explicit GUIContainer(Spacing margin, Padding padding, GUIContainer *parent);

    explicit GUIContainer(Padding padding, LayoutType layout =FreeformLayout, GUIContainer *parent =0);
    explicit GUIContainer(Padding padding, GUIContainer *parent);

    explicit GUIContainer(Spacing margin, LayoutType layout, GUIContainer *parent =0);
    explicit GUIContainer(Spacing margin, GUIContainer *parent);

    explicit GUIContainer(LayoutType layout, GUIContainer *parent =0);
    explicit GUIContainer(GUIContainer *parent =0);

    virtual void fixLayoutImpl();

protected slots:
    inline void fixLayout() {
        if(!wattr().testFlag(DirtyLayout))
            return;

        fixLayoutImpl();

        if(wattr().testFlag(DirtyLayout))
            setWAttr(wattr() ^ DirtyLayout);
    }

private:
    inline void init(LayoutType layout) {init(Spacing(), Padding(), layout);}
    inline void init(Spacing margin, LayoutType layout) {init(margin, Padding(), layout);}
    inline void init(Padding padding, LayoutType layout =FreeformLayout) {init(Spacing(), padding, layout);}
    void init(Spacing margin =Spacing(), Padding padding =Padding(), LayoutType layout =FreeformLayout);

    LayoutType _layout;
    QSize _preferredSize;

    QTimer layoutTimer;
    Padding _padding;
    Spacing _spacing;
};

#endif // GUICONTAINER_H
