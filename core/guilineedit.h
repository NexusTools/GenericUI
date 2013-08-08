#ifndef GUILINEEDIT_H
#define GUILINEEDIT_H

#include "guilabel.h"

class GUILineEdit : public GUILabel
{
    Q_OBJECT
public:
    inline GUILineEdit(QString text, GUIContainer* parent =0) : GUILabel(text, parent) {setWAttr(Focusable);}
    inline GUILineEdit(GUIContainer* parent =0) : GUILabel(parent) {setWAttr(Focusable);}

};

#endif // GUILINEEDIT_H
