#ifndef GUISLIDER_H
#define GUISLIDER_H

#include "genericui.h"

#include <QPair>

class GUISlider : public GUIWidget
{
    Q_OBJECT

public:
    typedef QPair<qreal,qreal> Bounds;

signals:
    void valueChanged();

private:
    Bounds _bounds;
    qreal _value;

};

#endif // GUISLIDER_H
