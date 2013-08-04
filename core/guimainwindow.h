#ifndef GUIMAINWINDOW_H
#define GUIMAINWINDOW_H

#include "guiwindow.h"

class GUIMainWindow : public GUIWindow
{
    Q_OBJECT
public:
    inline GUIMainWindow(QString title) : GUIWindow(title, 0) {}
};

#endif // GUIMAINWINDOW_H
