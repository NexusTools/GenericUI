#ifndef GUIMENUBAR_H
#define GUIMENUBAR_H

#include "guicontainer.h"
#include "guimenu.h"

class GUIAction;
class GUIMenu;

class GUIMenuBar : public GUIContainer
{
    Q_OBJECT
public:
    inline GUIMenuBar(GUIContainer* parent) : GUIContainer(HorizontalLayout, parent) {}

    inline void addAction(GUIAction* action) {action->setParent(this);}
    inline void addMenu(GUIMenu* menu, QString text) {addAction(menu->action(text));}
    inline void addMenu(GUIMenu* menu) {addAction(menu->action());}
    virtual void addSeparator() =0;

};

#endif // GUIMENUBAR_H
