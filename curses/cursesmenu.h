#ifndef CURSESMENU_H
#define CURSESMENU_H

#include <QPointer>

#include <guimenu.h>

#include "curseslabel.h"
#include "cursesaction.h"
#include "cursesmenuseparator.h"

class CursesMenu : public GUIMenu, public CursesWindow
{
    Q_OBJECT
    CURSES_WINDOW

    friend class CursesAction;
public:
    inline CursesMenu(QString title, GUIWindow* parent) : GUIMenu(title, Padding(QPoint(1,1),QPoint(1,1)), parent) {
        _size=0;
        _action=0;
        _open=false;
        _animationTimer.setInterval(40);
        connect(&_animationTimer, SIGNAL(timeout()), this, SLOT(animate()));
        setWAttr(WAttrs(NoAutoResize | Hidden));
    }

    virtual GUIAction* action(QString name =QString()) {
        if(name.isEmpty())
            name = title();

        _action = new CursesAction(name);
        _action->_menu = this;
        return _action.data();
    }

    inline void addAction(GUIAction* action) {action->setParent(this);}
    inline void addMenu(GUIMenu* menu, QString text) {addAction(menu->action(text));}
    inline void addMenu(GUIMenu* menu) {addAction(menu->action());}
    inline void addSeparator() {new CursesMenuSeparator(this);}

    virtual QSize sizeForLayout(int) {
        int h = 0;
        int w = 0;
        foreach(GUIWidget* child, childWidgets()) {
            if(child->isHidden())
                continue;

            QSize pref = child->preferredSize();
            h+=pref.height();
            if(pref.width() > w)
                w = pref.width();
        }

        return QSize(2+w,2+h);
    }

    bool isOpen();

protected:
    virtual void showImpl() {
        CursesWindow::showImpl();
        if(_action)
            _action->markDirty();

        _open = true;
        _animationTimer.start();
    }

    virtual void hideImpl() {
        _open = false;
        _animationTimer.start();
    }

protected slots:
    void animate() {
        if(_open) {
            _size += (1-_size+0.4)/4;

            if(_size >= 1) {
                _size = 1;
                _animationTimer.stop();
            }
        } else {
            _size -= (_size+0.4)/4;

            if(_size <= 0) {
                _size = 0;
                CursesWindow::hideImpl();

                if(_action)
                    _action->markDirty();

                _animationTimer.stop();
                return;
            }
        }

        QSize pref = preferredSize();
        pref = QSize((pref.width()-2)*_size, (pref.height()-2)*_size);
        if(pref.height() >= 0 || pref.width() >= 0)
        resize(pref + QSize(2, 2));
    }

    inline virtual void drawImpl() {
        wmove(hnd(), 0, 0);
        waddch(hnd(), ACS_ULCORNER);
        int left = width()-2;
        while(left > 0) {
            waddch(hnd(), ACS_HLINE);
            left--;
        }
        waddch(hnd(), ACS_URCORNER);

        for(int y=1; y<height()-1; y++) {
            mvwaddch(hnd(), y, 0, ACS_VLINE);
            mvwaddch(hnd(), y, width()-1, ACS_VLINE);
        }

        wmove(hnd(), height()-1, 0);
        waddch(hnd(), ACS_LLCORNER);
        left = width()-2;
        while(left > 0) {
            waddch(hnd(), ACS_HLINE);
            left--;
        }
        waddch(hnd(), ACS_LRCORNER);
    }

    virtual void drawChildren(QRect clip, QPoint off) {
        Padding pad = padding();
        clip &= QRect(pad.first, QPoint(width()-1, height()) - (pad.second+pad.first));

        Children children = childWidgets();
        foreach(GUIWidget* child, children) {
            if(child->isHidden() || child->isWindow())
                continue;

            CursesBase* base = dynamic_cast<CursesBase*>(child);
            if(base)
                drawChild(base, clip, off);
        }

    }

    virtual bool processEvent(QEvent*);
    void showChain();
    void hideChain();

private:
    bool _open;
    float _size;
    QTimer _animationTimer;
    QPointer<CursesAction> _action;
};

#endif // CURSESMENU_H
