#ifndef GUIWINDOW_H
#define GUIWINDOW_H

#include "guicontainer.h"

class GUIWindow : public GUIContainer
{
public:
    GUIWindow(QString title, GUIWindow* parent) {_title=title;_parent=parent;}

private:
    GUIWindow* _parent;
    QString _title;
};

#endif // GUIWINDOW_H
