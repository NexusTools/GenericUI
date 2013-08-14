#ifndef GUIACTION_H
#define GUIACTION_H

#include "guilabel.h"

class GUIAction : public GUILabel
{
    Q_OBJECT
public:
    virtual Qt::Key shortcut() =0;

protected:
    inline GUIAction(QString text, WAttrs attr, GUIContainer* parent =0) : GUILabel(text, parent) {setWAttr(attr | Focusable);}
    inline GUIAction(WAttrs attr, GUIContainer* parent =0) : GUILabel(parent) {setWAttr(attr | Focusable);}

signals:
    void activated();

private:

};

#endif // GUIACTION_H
