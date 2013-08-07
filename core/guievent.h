#ifndef GUIEVENT_H
#define GUIEVENT_H

#include <QEvent>
#include <QPoint>
#include <QList>

class GUIEvent : public QEvent
{
public:
    enum GUIType {
        GUITypeBase = User + 10000,

        // State
        GUIWAttrChanged,

        GUIStateChanged,
        GUIVisibilityChanged,
        GUILayoutBecameDirty,

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

        GUIUserType = GUITypeBase + 500
    };

    inline explicit GUIEvent(GUIType t) : QEvent((Type)t) {}
};

class GUIMouseEvent : public GUIEvent
{
    friend class GUIContainer;
public:
    enum Button {
        LeftButton,
        MiddleButton,
        RightButton
    };

    inline GUIMouseEvent(GUIEvent::GUIType t, Button b, QPoint p) : GUIEvent(t) {btn=b;_pos=p;}

    inline QPoint pos() const{return _pos;}
    inline Button button() const{return btn;}

protected:
    inline void push(int x, int y) {stack << _pos; _pos += QPoint(x, y);}
    inline void pop() {_pos = stack.takeLast();}

private:
    Button btn;
    QPoint _pos;
    QList<QPoint> stack;
};

class GUIKeyEvent : public GUIEvent
{
public:
    inline GUIKeyEvent(int key) : GUIEvent(GUIEvent::GUIKeyTyped) {_key=key;}
    inline int key() const{return _key;}

private:
    int _key;
};

#endif // GUIEVENT_H
