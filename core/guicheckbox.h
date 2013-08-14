#ifndef GUICHECKBOX_H
#define GUICHECKBOX_H

#include "guiaction.h"

class GUICheckBox : public GUIAction
{
    Q_OBJECT
public:
    inline GUICheckBox(QString text, WAttrs wattr, GUIContainer* parent =0) : GUIAction(text, wattr, parent) {}
    inline GUICheckBox(WAttrs wattr, GUIContainer* parent =0) : GUIAction(wattr, parent) {}


};

#endif // GUICHECKBOX_H
