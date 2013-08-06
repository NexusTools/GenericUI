#include "guiscrollview.h"
#include "guiscrollbar.h"

GUIScrollView::GUIScrollView(GUIScrollBar* v, GUIScrollBar* h, GUIContainer *parent)
            : GUIContainer(Spacing(0, 0), Padding(QPoint(0, 0),
                 QPoint(v->width(), v->height())), parent) {

    _vScrollBar = v;
    _hScrollBar = h;
    _content = 0;
}
