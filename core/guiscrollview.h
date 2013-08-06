#ifndef GUISCROLLVIEW_H
#define GUISCROLLVIEW_H

#include "guicontainer.h"

class GUIScrollBar;

class GUIScrollView : public GUIContainer
{
private:
    GUIScrollBar *_vScrollBar;
    GUIScrollBar *_hScrollBar;
    GUIWidget *_content;
};

#endif // GUISCROLLVIEW_H
