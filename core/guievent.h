#ifndef GUIEVENT_H
#define GUIEVENT_H

#include <QEvent>

class GUIEvent : public QEvent
{
public:
    enum GUIType {
        GUITypeBase = User + 10000,

        // Heirarchy
        GUIParentAboutToChange,
        GUIParentChanged,

        // State
        GUIStateChanged,
        GUIVisibilityChanged,
        GUIWAttrChanged,

        // Geometry
        GUIGeometryChanged,
        GUIPositionChanged,
        GUISizeChanged,

        GUIUserType = GUITypeBase + 5000
    };

    inline explicit GUIEvent(GUIType t) : QEvent((Type)t) {}
};

#endif // GUIEVENT_H
