#ifndef GUIACTION_H
#define GUIACTION_H

#include "genericui.h"

class GUIAction : public GUIWidget
{
    Q_OBJECT
public:
    inline GUIAction(QString text, GUIContainer* parent =0) : GUIWidget(parent) {_text=text;}
    inline GUIAction(GUIContainer* parent =0) : GUIWidget(parent) {}


protected:
    virtual void actionActivated() {}

signals:
    void activated();

private:
    QString _text;

};

#endif // GUIACTION_H
