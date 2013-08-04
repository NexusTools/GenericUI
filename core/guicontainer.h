#ifndef GUICONTAINER_H
#define GUICONTAINER_H

#include "genericui.h"

class GUIContainer : public GUIWidget
{
    Q_OBJECT

    friend class GUIWidget;
public:
    enum LayoutType {
        InlineElements,
        BlockElements,
        FreeformLayout
    };

    inline QList<GUIWidget*> children() const{return _children;}

protected:
    QSize sizeForLayout();

private:
    LayoutType _layout;
    QList<GUIWidget*> _children;
};

#endif // GUICONTAINER_H
