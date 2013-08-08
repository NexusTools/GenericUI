#include <guicontainer.h>

#include "cursesbuttonbox.h"
#include "cursescontainer.h"
#include "cursesmenubar.h"

QPointer<GUIWidget> CursesBase::_focusBase;

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

            if(kEv->mod().testFlag(Qt::AltModifier)) {
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
            widget()->simEvent(GUIEvent::GUIScreenPositionChanged);
        case GUIEvent::GUIVisibilityChanged:
            repaint();
            break;

        case GUIEvent::GUIMouseClicked:
            if(widget()->wattr().testFlag(GUIWidget::Focusable))
                giveFocus();
            break;

        case GUIEvent::GUIKeyTyped:
        {
            GUIContainer* container;
            if(container = widget()->parentContainer())
                container->internal<CursesBase>()->pushEvent(ev);
            break;
        }

        case GUIEvent::GUIWAttrChanged:
            if(widget()->wattr().testFlag(GUIWidget::Focused)) {
                if(!widget()->wattr().testFlag(GUIWidget::Focusable))
                    widget()->setWAttr(GUIWidget::Focused, false);
                else
                    giveFocus();
            }
            break;

        default:
            break;
    }

    return false;
}

void CursesBase::giveFocus() {
    if(_focusBase == widget())
        return;

    GUIWidget* oldFocus = _focusBase.data();
    _focusBase = widget();

    if(oldFocus) {
        oldFocus->setWAttr(GUIWidget::Focused, false);
        oldFocus->simEvent(GUIEvent::GUIFocusChanged);
        oldFocus->simEvent(GUIEvent::GUIFocusLost);
    }

    widget()->simEvent(GUIEvent::GUIFocusChanged);
    widget()->simEvent(GUIEvent::GUIFocusGained);
    widget()->setWAttr(GUIWidget::Focused);
}

GUIWidget* CursesBaseContainer::nextFocusable(GUIWidget *from, GUIWidget **first) {
    GUIContainer::Children chld = ((GUIContainer*)widget())->childWidgets();
    QListIterator<GUIWidget*> i(chld);

    while(i.hasNext()) {
        GUIWidget* next = i.next();
        GUIContainer* con = qobject_cast<GUIContainer*>(next);
        if(!con && !next->isFocusable())
            continue;
        if(first && !*first)
            *first = next;

        if(from == 0) {
            from = next;
            i.previous();
        }

        if(from == next || (from == (GUIWidget *)-1 && next->isFocused())) {
            while(i.hasNext()) {
                next = i.next();
                con = qobject_cast<GUIContainer*>(next);
                if(con) {
                    next = con->internal<CursesBaseContainer>()->nextFocusable(0, first);
                    if(next)
                        return next;
                    continue;
                }

                if(next->isFocusable())
                    return next;
            }

            break;
        }
    }

    GUIContainer* par = widget()->parentContainer();
    if(par)
        return par->internal<CursesBaseContainer>()->nextFocusable(widget());

    return 0;
}

bool CursesBaseContainer::processEvent(QEvent* ev) {
    switch(ev->type()) {
        case GUIEvent::GUISizeChanged:
            ((GUIContainer*)widget())->markLayoutDirty();
            break;

        case GUIEvent::GUIScreenPositionChanged:
        {
            foreach(QObject* obj, widget()->children()) {
                GUIWidget* child = qobject_cast<GUIWidget*>(obj);
                if(!child)
                    continue;

                child->event(ev);
            }

            break;
        }

        case GUIEvent::GUIKeyTyped:
        {
            GUIKeyEvent* kEv = (GUIKeyEvent*)ev;
            if(kEv->key() == Qt::Key_Tab) {
                GUIWidget* next = nextFocusable();
                if(next) {
                    next->focus();
                    return true;
                }
            }
        }

        default:
            break;
    }

    return CursesBase::processEvent(ev);
}


