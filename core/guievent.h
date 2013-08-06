#ifndef GUIEVENT_H
#define GUIEVENT_H

#include <QEvent>

class GUIEvent : public QEvent
{
public:
    enum GUIType {
        GUITypeBase = User + 10000,

        // State
        GUIWAttrChanged,

        GUIStateChanged,
        GUIVisibilityChanged,

        GUIFocusChanged,
        GUIFocusGained,
        GUIFocusLost,

        // Input
        GUIKeyTyped, // Raw entry fields
        GUITextChanged, // Input boxes and labels

        GUIMouseClicked,
        GUIMousePressed,
        GUIMouseReleased,

        GUIActivated,

        // Geometry
        GUIGeometryChanged,
        GUIPositionChanged,
        GUISizeChanged,

        GUIUserType = GUITypeBase + 5000
    };

    inline explicit GUIEvent(GUIType t) : QEvent((Type)t) {}
};

#endif // GUIEVENT_H
