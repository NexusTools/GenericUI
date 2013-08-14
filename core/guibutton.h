#ifndef GUIBUTTON_H
#define GUIBUTTON_H

#include "guiaction.h"

class GUIButton : public GUIAction
{
protected:
    inline GUIButton(QString text, WAttrs wattr, GUIContainer* parent =0) : GUIAction(text, wattr, parent) {}
    inline GUIButton(WAttrs wattr, GUIContainer* parent =0) : GUIAction(wattr, parent) {}

};

#endif // GUIBUTTON_H
