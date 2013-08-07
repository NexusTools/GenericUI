#ifndef CURSESDIALOG_H
#define CURSESDIALOG_H

#include <QTimer>

#include <guidialog.h>

#include "cursesbase.h"

class CursesDialog : public GUIDialog, public CursesWindow
{
    Q_OBJECT
    CURSES_WINDOW
public:
    inline explicit CursesDialog(QString title, GUIContainer *parent) : GUIDialog(title, Padding(QPoint(2, 3), QPoint(2, 1)), parent) {
        _size = 0;
        _open = false;
        _closable=true;
        setWAttr(Hidden);

        _animationTimer.setInterval(40);
        connect(&_animationTimer, SIGNAL(timeout()), this, SLOT(animate()));
    }

protected:
    virtual void drawChildren(QRect clip, QPoint off) {
        Padding pad = padding();
        clip &= QRect(pad.first, QPoint(width(), height()) - (pad.second+pad.first));

        Children children = childWidgets();
        foreach(GUIWidget* child, children) {
            if(child->isHidden() || child->isWindow())
                continue;

            CursesBase* base = dynamic_cast<CursesBase*>(child);
            if(base)
                drawChild(base, clip, off);
        }
    }

    inline virtual void drawImpl() {
        wmove(hnd(), 1, 1);
        waddstr(hnd(), qPrintable(title()));

        wmove(hnd(), 0, 0);
        waddch(hnd(), ACS_ULCORNER);
        int left = width()-2;
        while(left > 0) {
            waddch(hnd(), left == 2 && _closable ? ACS_TTEE : ACS_HLINE);
            left--;
        }
        waddch(hnd(), ACS_URCORNER);
        wmove(hnd(), 2, 1);
        left = width()-2;
        while(left > 0) {
            waddch(hnd(), left == 2 && _closable ? ACS_BTEE : ACS_HLINE);
            left--;
        }

        for(int y=1; y<height()-1; y++) {
            mvwaddch(hnd(), y, 0, y == 2 ? ACS_LTEE : ACS_VLINE);
            mvwaddch(hnd(), y, width()-1, y == 2 ? ACS_RTEE : ACS_VLINE);
        }

        wmove(hnd(), height()-1, 0);
        waddch(hnd(), ACS_LLCORNER);
        left = width()-2;
        while(left > 0) {
            waddch(hnd(), ACS_HLINE);
            left--;
        }
        waddch(hnd(), ACS_LRCORNER);

        if(!_closable)
            return;
        wmove(hnd(), 1, width()-3);
        waddch(hnd(), ACS_VLINE);
        waddch(hnd(), 'X');
    }

    virtual void showImpl() {
        CursesWindow::showImpl();

        _open = true;
        _animationTimer.start();
    }

    virtual void hideImpl() {
        _open = false;
        _animationTimer.start();
    }

    virtual bool processEvent(QEvent*);

protected slots:
    void animate() {
        setWAttr(WAttrs(wattr() | NoAutoResize));

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

                _animationTimer.stop();
                return;
            }
        }

        QSize pref = preferredSize();
        pref = QSize((pref.width()-2)*_size, (pref.height()-2)*_size);
        if(pref.height() >= 0 || pref.width() >= 0)
        resize(pref + QSize(2, 2));

        if(_size == 1)
            setWAttr(WAttrs(wattr() ^ NoAutoResize));
    }

    void center();

private:
    bool _open;
    bool _closable;
    float _size;

    QTimer _animationTimer;

};

#endif // CURSESDIALOG_H
