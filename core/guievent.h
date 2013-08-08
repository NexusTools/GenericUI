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
        GUIScreenPositionChanged,
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
    Qt::KeyboardModifiers _mod;
};

class GUIKeyEvent : public GUIEvent
{
public:
    inline GUIKeyEvent(Qt::Key key, Qt::KeyboardModifiers mod = Qt::NoModifier) : GUIEvent(GUIEvent::GUIKeyTyped) {_key=key;_mod=mod;}

    inline Qt::KeyboardModifiers mod() const{return _mod;}
    inline Qt::Key key() const{return _key;}

private:
    Qt::Key _key;
    Qt::KeyboardModifiers _mod;
};

#endif // GUIEVENT_H
