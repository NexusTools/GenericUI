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

        InlineElements,
        BlockElements
    };

    virtual QSize preferredSize();
    virtual QSize sizeForLayout(int maxWidth =0);
    inline Children childWidgets() const{return findChildren<GUIWidget*>("", Qt::FindDirectChildrenOnly);}

    virtual bool event(QEvent *);

protected:
    explicit GUIContainer(Spacing margin, Padding padding, LayoutType layout =FreeformLayout, GUIContainer *parent =0);
    explicit GUIContainer(Spacing margin, Padding padding, GUIContainer *parent);
    explicit GUIContainer(LayoutType layout, GUIContainer *parent =0);
    explicit GUIContainer(GUIContainer *parent =0);

    virtual bool eventFilter(QObject *, QEvent *);
    virtual void fixLayoutImpl();

protected slots:
    inline void fixLayout() {
        qDebug() << this << "Fixing layout";

        fixLayoutImpl();
    }
    inline void markLayoutDirty() {
        if(_dirtyLayout)
            return;

        qDebug() << this << "Marked layout dirty";
        _preferredSize = QSize();
        layoutTimer.start();
        _dirtyLayout = true;
    }

private:
    bool _dirtyLayout;
    LayoutType _layout;
    QSize _preferredSize;

    QTimer layoutTimer;
    Padding _padding;
    Spacing _spacing;
};

#endif // GUICONTAINER_H
