#ifndef GUIWINDOW_H
#define GUIWINDOW_H

#include "guicontainer.h"

class GUIWindow : public GUIContainer
{
    Q_OBJECT
public:
    GUIWindow(QString title, GUIWindow* parent) {_title=title;_parent=parent;}

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
    virtual void titleChanged() {}

private:
    GUIWindow* _parent;
    QString _title;
};

#endif // GUIWINDOW_H
