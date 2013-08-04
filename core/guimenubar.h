#ifndef GUIMENUBAR_H
#define GUIMENUBAR_H

#include "guicontainer.h"

class GUIMenuBar : public GUIContainer
{
    Q_OBJECT
public:
    inline GUIMenuBar(GUIContainer *parent =0) : GUIContainer(InlineElements, parent) {}

};

#endif // GUIMENUBAR_H
