#ifndef GUIACTION_H
#define GUIACTION_H

#include "guilabel.h"

class GUIAction : public GUILabel
{
    Q_OBJECT
public:
    inline GUIAction(QString text, GUIContainer* parent =0) : GUILabel(text, parent) {}
    inline GUIAction(GUIContainer* parent =0) : GUILabel(parent) {}

private:
    char shortcut;

};

#endif // GUIACTION_H
