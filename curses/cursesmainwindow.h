#ifndef CURSESMAINWINDOW_H
#define CURSESMAINWINDOW_H

#include <guimainwindow.h>
#include <QTimer>

#include "cursesbase.h"

class CursesMainWindow : public GUIMainWindow, public CursesScreen
{
    Q_OBJECT
public:
    inline explicit CursesMainWindow(QString title) : GUIMainWindow(title), CursesScreen() {
        init();
        resize(checkSize());

        repaintTimer.setInterval(0);
        repaintTimer.setSingleShot(true);
        connect(&repaintTimer, SIGNAL(timeout()), this, SLOT(drawNow()));
        repaintTimer.start();
    }

    inline void notifyDirty() {
        repaintTimer.start();
    }

    inline QRect geom() const{return GUIWidget::geom();}
    inline void parentChanged() {CursesBase::updateParent((CursesBase*)parentContainer());}

protected:
    inline void drawChildren() {
        foreach(GUIWidget* child, children()) {
            CursesBase* base = dynamic_cast<CursesBase*>(child);
            if(base)
                base->render(this);
        }
    }

    inline void sizeChanged() {notifyDirty();}

protected slots:
    inline void drawNow() {
        render(0);
    }

private:
    static void init();

    QTimer repaintTimer;
};

#endif // CURSESMAINWINDOW_H
