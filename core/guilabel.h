#ifndef GUILABEL_H
#define GUILABEL_H

#include "genericui.h"

class GUILabel : public GUIWidget
{
    Q_OBJECT
public:
    inline GUILabel(QString text, GUIContainer* parent =0) : GUIWidget(parent) {_text=text;}
    inline GUILabel(GUIContainer* parent =0) : GUIWidget(parent) {}

    inline void setText(QString text){
        if(text == _text)
            return;

        _text=text;
        fitToContent();
        textChanged();
    }

    virtual QSize preferredSize() {
        return sizeForString(text());
    }

    inline QString text() const{return _text;}

protected:
    virtual QSize sizeForString(QString) =0;
    virtual void textChanged() =0;

private:
    QString _text;
};

#endif // GUILABEL_H
