#ifndef GUIWINDOW_H
#define GUIWINDOW_H

#include "guicontainer.h"

class GUIWindow : public GUIContainer
{
    Q_OBJECT
public:
    inline bool isWindow() const{return true;}
    inline QString title() const{return _title;}

public slots:
    void setTitle(QString title) {
        if(_title == title)
            return;
        _title = title;
        titleChanged();
    }

    void close() {
        hide();
    }

protected:
    explicit GUIWindow(QString title, Spacing margin =Spacing(), Padding padding =Padding(), LayoutType layout =FreeformLayout, GUIContainer *parent =0) : GUIContainer(margin, padding, layout, parent) {_title=title;}
    explicit GUIWindow(QString title, Spacing margin, Padding padding, GUIContainer *parent) : GUIContainer(margin, padding, parent) {_title=title;}

    explicit GUIWindow(QString title, Padding padding, LayoutType layout =FreeformLayout, GUIContainer *parent =0) : GUIContainer(padding, layout, parent) {_title=title;}
    explicit GUIWindow(QString title, Padding padding, GUIContainer *parent) : GUIContainer(padding, parent) {_title=title;}
    explicit GUIWindow(QString title, LayoutType layout, GUIContainer *parent) : GUIContainer(layout, parent) {_title=title;}
    explicit GUIWindow(QString title, GUIContainer *parent) : GUIContainer(parent) {_title=title;}

    virtual void titleChanged() {}

private:
    GUIWindow* _parent;
    QString _title;
};

#endif // GUIWINDOW_H
