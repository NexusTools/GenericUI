#include <guicontainer.h>

#include "cursesbase.h"

CursesBase* CursesBase::_focusBase = 0;

void CursesScreen::processMouseEvent(MEVENT &mEvent)  {
    if(_cursor.isNull())
        curs_set(1);
    _cursor = QPoint(mEvent.x, mEvent.y);
    repaint();

    if(mEvent.bstate & BUTTON1_CLICKED) {
        GUIMouseEvent mEv(GUIEvent::GUIMouseClicked, GUIMouseEvent::LeftButton, _cursor);
        widget()->event(&mEv);
    }
}

bool CursesWindow::processEvent(QEvent *ev) {
    if(ev->type() == GUIEvent::GUIVisibilityChanged) {
        if(widget()->isHidden())
            hideImpl();
        else
            showImpl();
    }

    return CursesContainer::processEvent(ev);
}

void CursesBase::repaint() {
    CursesContainer* container = widget()->parentInternal<CursesContainer>();
    if(container)
        container->repaint();
}

void CursesBase::markDirty() {
    repaint();

    if(_dirty)
        return;

    _dirty = true;
}

bool CursesBase::processEvent(QEvent* ev) {
    switch(ev->type()) {
        case GUIEvent::GUISizeChanged:
            setSize(geom().size());
            break;

        case GUIEvent::GUIGeometryChanged:
        {
            GUIContainer* container = widget()->parentContainer();
            if(container)
                container->markLayoutDirty();
            break;
        }

        case GUIEvent::GUITextChanged:
            markDirty();
            break;

        case GUIEvent::GUIPositionChanged:
        case GUIEvent::GUIVisibilityChanged:
            repaint();
            break;

        default:
            break;
    }

    return false;
}

bool CursesContainer::processEvent(QEvent* ev) {
    switch(ev->type()) {
        case GUIEvent::GUISizeChanged:
            ((GUIContainer*)widget())->markLayoutDirty();
            break;

        default:
            break;
    }

    return CursesBase::processEvent(ev);
}


