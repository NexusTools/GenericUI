#ifndef GUIDIALOG_H
#define GUIDIALOG_H

#include "guiwindow.h"

class GUIDialog : public GUIWindow
{
    Q_OBJECT
protected:
    explicit GUIDialog(QString title, Spacing margin =Spacing(), Padding padding =Padding(), LayoutType layout =FreeformLayout, GUIContainer *parent =0) : GUIWindow(title, margin, padding, layout, parent) {}
    explicit GUIDialog(QString title, Spacing margin, Padding padding, GUIContainer *parent) : GUIWindow(title, margin, padding, parent) {}

    explicit GUIDialog(QString title, Padding padding, LayoutType layout =FreeformLayout, GUIContainer *parent =0) : GUIWindow(title, padding, layout, parent) {}
    explicit GUIDialog(QString title, Padding padding, GUIContainer *parent) : GUIWindow(title, padding, parent) {}
    explicit GUIDialog(QString title, LayoutType layout, GUIContainer *parent) : GUIWindow(title, layout, parent) {}
    explicit GUIDialog(QString title, GUIContainer *parent) : GUIWindow(title, parent) {}

};

#endif // GUIDIALOG_H
