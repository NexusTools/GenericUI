#include "cursesbase.h"

CursesBase* CursesBase::_focusBase = 0;

bool CursesBase::processEvent(QObject*, QEvent* ev) {
    switch(ev->type()) {
        case GUIEvent::GUISizeChanged:
            setSize(geom().size());
            break;

        default:
            break;
    }

    return false;
}


