#ifndef CURSESBUTTONBOX_H
#define CURSESBUTTONBOX_H

#include "cursescontainer.h"

class CursesButtonBox : public CursesContainer
{
    Q_OBJECT
public:
    inline explicit CursesButtonBox(GUIContainer* par =0) : CursesContainer(Spacing(1, 0), Padding(QPoint(0,1),QPoint(0,0)), HorizontalLayout, par) {expandToParent();}


protected:
    virtual bool processEvent(QEvent *);

};

#endif // CURSESBUTTONBOX_H
