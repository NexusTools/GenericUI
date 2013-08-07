#ifndef GUIACTION_H
#define GUIACTION_H

#include "guilabel.h"

class GUIAction : public GUILabel
{
    Q_OBJECT
protected:
    inline GUIAction(QString text, WAttrs attr, GUIContainer* parent =0) : GUILabel(text, parent) {setWAttr(attr | Focusable);}
    inline GUIAction(WAttrs attr, GUIContainer* parent =0) : GUILabel(parent) {setWAttr(attr | Focusable);}

signals:
    void activated();

private:
    char shortcut;

};

#endif // GUIACTION_H
