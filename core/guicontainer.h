#ifndef GUICONTAINER_H
#define GUICONTAINER_H

#include "genericui.h"

class GUIContainer : public GUIWidget
{
public:
    enum LayoutType {
        InlineElements,
        BlockElements,
        FreeformLayout
    };

protected:
    QSize sizeForLayout();

private:
    LayoutType _layout;
};

#endif // GUICONTAINER_H
