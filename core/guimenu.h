#ifndef GUIMENU_H
#define GUIMENU_H

#include "guiwindow.h"
#include "guiaction.h"

class GUIAction;

class GUIMenu : public GUIWindow
{
    Q_OBJECT
public:
    virtual GUIAction* action(QString name =QString()) =0;
    inline void addAction(GUIAction* action) {action->setParent(this);}
    inline void addMenu(GUIMenu* menu, QString text) {addAction(menu->action(text));}
    inline void addMenu(GUIMenu* menu) {addAction(menu->action());}
    virtual void addSeparator() =0;

protected:
    explicit GUIMenu(QString title, Spacing margin =Spacing(), Padding padding =Padding(), GUIContainer *parent =0) : GUIWindow(title, margin, padding, VerticalLayout, parent) {}
    explicit GUIMenu(QString title, Padding padding, GUIContainer *parent =0) : GUIWindow(title, padding, VerticalLayout, parent) {}
    explicit GUIMenu(QString title, GUIContainer *parent) : GUIWindow(title, VerticalLayout, parent) {}


};

#endif // GUIMENU_H
