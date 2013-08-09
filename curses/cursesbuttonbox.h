#ifndef CURSESBUTTONBOX_H
#define CURSESBUTTONBOX_H

#include "cursescontainer.h"

class CursesButtonBox : public CursesContainer
{
    Q_OBJECT
public:
    inline explicit CursesButtonBox(GUIContainer* par =0) : CursesContainer(Spacing(1, 0), HorizontalLayout, par) {expandToParent();}

public slots:
    bool activate();

};

#endif // CURSESBUTTONBOX_H
