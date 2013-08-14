#ifndef GUISCROLLVIEW_H
#define GUISCROLLVIEW_H

#include "guicontainer.h"

class GUIScrollBar;

class GUIScrollView : public GUIContainer
{
protected:
    explicit GUIScrollView(GUIScrollBar* v, GUIScrollBar* h, GUIContainer* =0);

private:
    GUIScrollBar *_vScrollBar;
    GUIScrollBar *_hScrollBar;
    GUIWidget *_content;

};

#endif // GUISCROLLVIEW_H
