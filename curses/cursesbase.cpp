#include <guicontainer.h>

#include "cursesbuttonbox.h"
#include "cursescontainer.h"
#include "cursesmenubar.h"

CursesBase* CursesBase::_focusBase = 0;

CursesBase::CursesBase(QSize size, int colorPair) {
    _window=newpad(size.height(), size.width());
    if(!_window)
        throw "Unable to allocate pad";
    wbkgd(_window, COLOR_PAIR(colorPair));
    _dirty=true;
}

void CursesScreen::processMouseEvent(MEVENT &mEvent)  {
    QPoint cur = QPoint(mEvent.x, mEvent.y);

    if(mEvent.bstate & BUTTON1_CLICKED) {
        GUIMouseEvent mEv(GUIEvent::GUIMouseClicked, GUIMouseEvent::LeftButton, cur);
        widget()->event(&mEv);
    }
}

bool CursesWindow::processEvent(QEvent *ev) {
    switch(ev->type()) {
        case GUIEvent::GUIVisibilityChanged:
            if(widget()->isHidden())
                hideImpl();
            else
                showImpl();
            break;

        case GUIEvent::GUIKeyTyped:
        {
            GUIKeyEvent* kEv = (GUIKeyEvent*)ev;
            if(kEv->key() == Qt::Key_Escape) {
                widget()->hide();
                return true;
            }

            if(kEv->mod().testFlag(Qt::ControlModifier)) {
                CursesMenuBar* menuBar = widget()->findChild<CursesMenuBar*>();
                if(menuBar)
                    if(menuBar->event(kEv))
                        return true;
                CursesButtonBox* buttonBox = widget()->findChild<CursesButtonBox*>();
                if(buttonBox)
                    if(buttonBox->event(kEv))
                        return true;
            }
            break;
        }

        default:
            break;

    }

    return CursesBaseContainer::processEvent(ev);
}

void CursesBase::repaint() {
    CursesBaseContainer* container = widget()->parentInternal<CursesBaseContainer>();
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

bool CursesBaseContainer::processEvent(QEvent* ev) {
    switch(ev->type()) {
        case GUIEvent::GUISizeChanged:
            ((GUIContainer*)widget())->markLayoutDirty();
            break;

        default:
            break;
    }

    return CursesBase::processEvent(ev);
}


