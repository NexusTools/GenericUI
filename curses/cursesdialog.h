#ifndef CURSESDIALOG_H
#define CURSESDIALOG_H

#include <QTimer>
#include <QVariant>
#include <QEventLoop>
#include <QCryptographicHash>
#include <QStringList>
#include <QSettings>

#include <guidialog.h>

#include "curseslineedit.h"
#include "cursesbuttonbox.h"
#include "cursesmainwindow.h"
#include "cursescontainer.h"
#include "cursescheckbox.h"
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
        setWAttr(WAttrs(NoAutoResize | Hidden));

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

    inline static bool ensure(QString text, QString title ="Decide", QSettings* config =0) {
        return options(QStringList() << "_Yes" << "_No", text, title, config) == "Yes";
    }

    inline static bool continu(QString text, QString title ="Continue?", QSettings* config =0) {
        return options(QStringList() << "C_ontinue" << "Clos_e", text, title, config) == "Continue";
    }

    inline static QString input(QString text, QString title ="Input", QString def ="") {
        CursesDialog* diag = new CursesDialog(title, CursesMainWindow::current());
        connect(diag, SIGNAL(finished()), diag, SLOT(deleteLater()));

        CursesVBox* vBox = new CursesVBox(diag);
        new CursesLabel(text, vBox);
        CursesLineEdit* lineEdit = new CursesLineEdit(def, vBox);
        lineEdit->setWAttr(GUIWidget::ExpandWidth);

        CursesButtonBox* buttonContainer = new CursesButtonBox(diag);
        foreach(QString option, QStringList() << "O_kay" << "Ca_ncel") {
            CursesButton* act = new CursesButton(option, GUIWidget::FloatCenter, buttonContainer);
            connect(act, SIGNAL(selected(QVariant)), diag, SLOT(answer(QVariant)));
        }

        diag->setLayout(GUIContainer::VerticalLayout);
        diag->exec();

        QString val = diag->value<QString>();
        return val == "Okay" ? lineEdit->text() : "";
    }

    inline static QString options(QStringList options, QString text ="Select an option.", QString title ="Options", QSettings* config =0) {
        QString uid;
        if(config) {
            uid += '[';
            QStringList fixedOptions;
            foreach(QString option, options) {
                option = option.remove('_');
                fixedOptions << option;
                uid += option;
            }
            uid += ']';
            uid += text;
            uid += ':';
            uid += title;

            uid = QString(QCryptographicHash::hash(uid.toLocal8Bit(), QCryptographicHash::Md5).toHex());

            config->beginGroup("saved-dialogs");
            QString saved = config->value(uid).toString();
            if(!saved.isEmpty() && fixedOptions.contains(saved)) {
                config->endGroup();
                return saved;
            }
        }

        CursesDialog* diag = new CursesDialog(title, CursesMainWindow::current());
        connect(diag, SIGNAL(finished()), diag, SLOT(deleteLater()));

        CursesVBox* vBox = new CursesVBox(diag);
        foreach(QString line, text.split('\n'))
            new CursesLabel(line, vBox);

        CursesCheckBox* checkBox;
        if(config)
            checkBox = new CursesCheckBox("Don't ask again", diag);

        CursesButtonBox* buttonContainer = new CursesButtonBox(diag);
        foreach(QString option, options) {
            CursesButton* act = new CursesButton(option, GUIWidget::FloatCenter, buttonContainer);
            connect(act, SIGNAL(selected(QVariant)), diag, SLOT(answer(QVariant)));
        }

        diag->setLayout(GUIContainer::VerticalLayout);
        diag->exec();

        QString val = diag->value<QString>();
        if(config) {
            if(checkBox->isChecked())
                config->setValue(uid, val);
            config->endGroup();
        }

        return val;
    }

    inline bool hasValue() const{
        return _value.isValid();
    }

    inline void clearValue() {
        _value.clear();
    }

    template <class T>
    inline T value() {
        return _value.value<T>();
    }

    inline bool isExpanded() {
        return _size > 0;
    }

public slots:
    void answer(QVariant val) {_value=val;close();emit answered();}

protected:
    virtual void drawChildren(QRect clip, QPoint off) {
        Padding pad = padding();
        clip &= QRect(pad.first, QPoint(width(), height()+1) - (pad.second+pad.first));

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
        setWAttr(WAttrs(wattr() | NoAutoResize));

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
        setWAttr(WAttrs(wattr() | NoAutoResize));

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

        if(_size == 1 && wattr().testFlag(NoAutoResize))
            setWAttr(WAttrs(wattr() ^ NoAutoResize));
    }

    void center();

signals:
    void answered();
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
