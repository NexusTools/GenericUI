#ifndef GUIMENU_H
#define GUIMENU_H

#include "guiwindow.h"
#include "guiaction.h"

class GUIAction;

class GUIMenu : public GUIWindow
{
    Q_OBJECT
public:
    inline GUIMenu(QString title, GUIWindow* parent) : GUIWindow(title, parent) {}

    virtual GUIAction* action(QString name =QString()) =0;
    inline void addAction(GUIAction* action) {action->setParent(this);}
    inline void addMenu(GUIMenu* menu, QString text) {addAction(menu->action(text));}
    inline void addMenu(GUIMenu* menu) {addAction(menu->action());}
    virtual void addSeparator() =0;

};

#endif // GUIMENU_H
