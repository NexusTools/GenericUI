#ifndef CURSESDIALOG_H
#define CURSESDIALOG_H

#include <QTimer>
#include <QEventLoop>
#include <QStringList>

#include <guidialog.h>

#include "cursesmainwindow.h"
#include "curseslabel.h"

class CursesDialog : public GUIDialog, public CursesWindow
{
    Q_OBJECT
    CURSES_WINDOW
public:
    inline explicit CursesDialog(QString title, GUIContainer *parent) : GUIDialog(title, Spacing(1, 0), Padding(QPoint(2, 3), QPoint(2, 1)), parent) {
        _size = 0;
        _open = false;
        _closable=true;
        setWAttr(Hidden);

        _animationTimer.setInterval(40);
        connect(&_animationTimer, SIGNAL(timeout()), this, SLOT(animate()));
    }
    virtual ~CursesDialog() {hideImpl();}

    inline static void alert(QString text, QString title ="Alert") {
        CursesDialog* diag = new CursesDialog(title, CursesMainWindow::current());
        connect(diag, SIGNAL(finished()), diag, SLOT(deleteLater()));

        new CursesLabel(text, diag);
        CursesAction* act = new CursesAction("[ Okay ]", diag);
        connect(act, SIGNAL(activated()), diag, SLOT(close()));
        diag->setLayout(GUIContainer::VerticalLayout);
        diag->show();
        return;
    }

    inline static QString options(QStringList options, QString text ="Select an option.", QString title ="Options") {
        CursesDialog* diag = new CursesDialog(title, CursesMainWindow::current());
        connect(diag, SIGNAL(finished()), diag, SLOT(deleteLater()));

        new CursesLabel(text, diag);
        foreach(QString option, options) {
            CursesAction* act = new CursesAction(QString("[ %1 ]").arg(option), diag);
            connect(act, SIGNAL(activated()), diag, SLOT(close()));
        }

        diag->setLayout(GUIContainer::VerticalLayout);
        diag->show();

        QEventLoop eventLoop;
        while(!diag->hasValue())
            eventLoop.processEvents(QEventLoop::WaitForMoreEvents);

        return diag->value<QString>();
    }

    inline bool hasValue() const{
        return _value.isValid();
    }

    template <class T>
    inline T value() {
        return _value.value<T>();
    }

public slots:
    void answer(QVariant val) {_value=val;close();}

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

    inline virtual void drawImpl() {
        wmove(hnd(), 1, 1);
        waddstr(hnd(), qPrintable(title()));

        if(_closable) {
            wmove(hnd(), 1, width()-3);
            waddch(hnd(), ACS_VLINE);
            waddch(hnd(), 'X');
        }
        wmove(hnd(), 2, 1);
        int left = width()-2;
        while(left > 0) {
            waddch(hnd(), left == 2 && _closable ? ACS_BTEE : ACS_HLINE);
            left--;
        }


        wmove(hnd(), 0, 0);
        waddch(hnd(), ACS_ULCORNER);
        left = width()-2;
        while(left > 0) {
            waddch(hnd(), left == 2 && _closable ? ACS_TTEE : ACS_HLINE);
            left--;
        }
        waddch(hnd(), ACS_URCORNER);

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
                if(hasValue())
                    emit finished(_value);
                emit finished();
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

signals:
    void finished();
    void finished(QVariant value);

private:
    bool _open;
    bool _closable;
    float _size;

    QVariant _value;
    QTimer _animationTimer;

};

#endif // CURSESDIALOG_H
