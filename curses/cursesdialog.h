#ifndef CURSESDIALOG_H
#define CURSESDIALOG_H

#include <QTimer>
#include <QVariant>
#include <QEventLoop>
#include <QStringList>

#include <guidialog.h>

#include "cursesbuttonbox.h"
#include "cursesmainwindow.h"
#include "cursescontainer.h"
#include "cursesbutton.h"
#include "curseslabel.h"

class CursesDialog : public GUIDialog, public CursesWindow
{
    Q_OBJECT
    CURSES_WINDOW
public:
    inline explicit CursesDialog(QString title, GUIContainer *parent) : GUIDialog(title, Spacing(1, 1), Padding(QPoint(2, 2), QPoint(2, 1)), parent) {
        _size = 0;
        _open = false;
        _closable=true;
        setWAttr(Hidden);

        _animationTimer.setInterval(40);
        connect(&_animationTimer, SIGNAL(timeout()), this, SLOT(animate()));
    }
    virtual ~CursesDialog() {hideImpl();}

    inline void exec() {
        _value.clear();
        show();

        QEventLoop eventLoop;
        while(!hasValue())
            eventLoop.processEvents(QEventLoop::WaitForMoreEvents);
    }

    inline static void alert(QString text, QString title ="Alert") {
        options(QStringList() << "O_kay", text, title);
    }

    inline static bool ensure(QString text, QString title ="Decide") {
        return options(QStringList() << "_Yes" << "_No", text, title) == "Yes";
    }

    inline static QString options(QStringList options, QString text ="Select an option.", QString title ="Options") {
        CursesDialog* diag = new CursesDialog(title, CursesMainWindow::current());
        connect(diag, SIGNAL(finished()), diag, SLOT(deleteLater()));

        new CursesLabel(text, diag);
        CursesButtonBox* buttonContainer = new CursesButtonBox(diag);
        foreach(QString option, options) {
            CursesButton* act = new CursesButton(option, GUIWidget::FloatCenter, buttonContainer);
            connect(act, SIGNAL(selected(QVariant)), diag, SLOT(answer(QVariant)));
        }

        diag->setLayout(GUIContainer::VerticalLayout);
        diag->exec();

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
        wmove(hnd(), 0, 0);
        waddch(hnd(), ACS_ULCORNER);
        int left = width()-2;
        while(left > 0) {
            waddch(hnd(), ACS_HLINE);
            left--;
        }

        wmove(hnd(), 0, 2);
        wattrset(hnd(), A_STANDOUT);
        waddch(hnd(), ' ');
        waddstr(hnd(), qPrintable(title()));
        waddch(hnd(), ' ');

        if(_closable) {
            wmove(hnd(), 0, width()-5);
            waddstr(hnd(), " X ");
            wattrset(hnd(), A_NORMAL);
            waddch(hnd(), ACS_HLINE);
            waddch(hnd(), ACS_URCORNER);
        } else {
            wattrset(hnd(), A_NORMAL);
            wmove(hnd(), 0, width()-1);
            waddch(hnd(), ACS_URCORNER);
        }

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

    virtual QSize preferredSize() {
        QSize s = GUIDialog::preferredSize();
        s.setWidth(qMax(s.width(), 40));

        return QSize(s);
    }

    virtual void showImpl() {
        _lastFocus = currentFocus();
        CursesWindow::showImpl();
        focusFirstChild(this);

        _open = true;
        _animationTimer.start();
    }

    static inline bool focusFirstChild(GUIContainer* container) {
        foreach(QObject* obj, container->children()) {
            GUIWidget* child = qobject_cast<GUIWidget*>(obj);
            if(child && child->isFocusable()) {
                child->focus();
                return true;
            }

            GUIContainer* subContainer = qobject_cast<GUIContainer*>(obj);
            if(subContainer && focusFirstChild(subContainer))
                return true;
        }

        return false;
    }

    virtual void hideImpl() {
        _open = false;
        _animationTimer.start();

        if(!_lastFocus.isNull()) {
            _lastFocus->focus();
            _lastFocus = 0;
        }
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
    QPointer<GUIWidget> _lastFocus;
    QTimer _animationTimer;

};

#endif // CURSESDIALOG_H
