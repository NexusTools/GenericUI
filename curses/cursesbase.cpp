#include "cursesbase.h"

CursesBase* CursesBase::_focusBase = 0;

bool CursesBase::processEvent(QObject*, QEvent* ev) {
    switch(ev->type()) {
        case GUIEvent::GUISizeChanged:
            setSize(geom().size());
            break;

        case GUIEvent::GUIPositionChanged:
            cursesDirtyMainWindow();
            break;

        case GUIEvent::GUITextChanged:
        case GUIEvent::GUIWAttrChanged:
            markDirty();
            break;

        default:
            break;
    }

    return false;
}


