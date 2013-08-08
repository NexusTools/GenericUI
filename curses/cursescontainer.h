#ifndef CURSESCONTAINER_H
#define CURSESCONTAINER_H

#include <guicontainer.h>
#include "cursesbase.h"

class CursesContainer : public GUIContainer, public CursesBaseContainer
{
    Q_OBJECT
    CURSES_CONTAINER
public:
    inline explicit CursesContainer(Spacing margin, Padding padding =Padding(), LayoutType layout =FreeformLayout, GUIContainer *parent =0) : GUIContainer(margin, padding, layout, parent) {}
    inline explicit CursesContainer(Spacing margin, Padding padding, GUIContainer *parent) : GUIContainer(margin, padding, parent) {}

    inline explicit CursesContainer(Padding padding, LayoutType layout =FreeformLayout, GUIContainer *parent =0) : GUIContainer(padding, layout, parent) {}
    inline explicit CursesContainer(Padding padding, GUIContainer *parent) : GUIContainer(padding, parent) {}

    inline explicit CursesContainer(Spacing margin, LayoutType layout, GUIContainer *parent =0) : GUIContainer(margin, layout, parent) {}
    inline explicit CursesContainer(Spacing margin, GUIContainer *parent) : GUIContainer(margin, parent) {}

    inline explicit CursesContainer(LayoutType layout, GUIContainer *parent =0) : GUIContainer(layout, parent) {}
    inline explicit CursesContainer(GUIContainer *parent =0) : GUIContainer(parent) {}

    inline void expandToParent() {
        setWAttr(GUIWidget::NoAutoResize);
        setWAttr(GUIWidget::ExpandHeight);
        setWAttr(GUIWidget::ExpandWidth);
    }

protected:
    virtual void drawChildren(QRect clip, QPoint off) {
        Children children = childWidgets();
        foreach(GUIWidget* child, children) {
            if(child->isHidden() || child->isWindow())
                continue;

            CursesBase* base = dynamic_cast<CursesBase*>(child);
            if(base)
                drawChild(base, clip, off);
        }

    }
};

class CursesVBox : public CursesContainer
{
public:
    inline explicit CursesVBox(Spacing margin, Padding padding =Padding(QPoint(0,0),QPoint(0,0)), GUIContainer *parent =0) : CursesContainer(margin, padding, VerticalLayout, parent) {expandToParent();}
    inline explicit CursesVBox(Padding padding, GUIContainer *parent =0) : CursesContainer(padding, VerticalLayout, parent) {expandToParent();}
    inline explicit CursesVBox(GUIContainer* par =0) : CursesContainer(VerticalLayout, par) {expandToParent();}

};

class CursesHBox : public CursesContainer
{
public:
    inline explicit CursesHBox(Spacing margin, Padding padding =Padding(QPoint(0,0),QPoint(0,0)), GUIContainer *parent =0) : CursesContainer(margin, padding, HorizontalLayout, parent) {expandToParent();}
    inline explicit CursesHBox(Padding padding, GUIContainer *parent =0) : CursesContainer(padding, HorizontalLayout, parent) {expandToParent();}
    inline explicit CursesHBox(Spacing margin, GUIContainer *parent =0) : CursesContainer(margin, HorizontalLayout, parent) {expandToParent();}
    inline explicit CursesHBox(GUIContainer* par =0) : CursesContainer(HorizontalLayout, par) {expandToParent();}

};

#endif // CURSESCONTAINER_H
