#ifndef CURSESWINDOW_H
#define CURSESWINDOW_H

#include <ncurses.h>

#include <QDebug>
#include <QRect>
#include <QList>

class GUIMainWindow;

void cursesDirtyMainWindow();

class CursesBase
{
    friend class CursesContainer;
    friend class CursesScreen;

    enum WinType {
        None,

        Screen,
        Window,
        Derived
    };

public:
    virtual ~CursesBase() {clear();if(_focusBase == this)_focusBase=0;}

    inline WINDOW* hnd() const{return _window;}

    inline void render(CursesBase* par) {if(!isValid())create(par);draw();}
    inline bool isValid() const{return _window;}
    inline bool isDirty() const{return _dirty;}
    inline void markDirty() {_dirty=true;notifyDirty();}
    virtual void notifyDirty() =0;

    inline void focus() {
        if(_focusBase)
            _focusBase->focusTaken();
        _focusBase = static_cast<CursesBase*>(this);
        _focusBase->focusGiven();
    }

    virtual QRect geom() const =0;
    inline virtual void updateParent(CursesBase* par) {
        if(_window)
            CursesBase::clear();
        if(par)
            create(par);
    }
    virtual bool isScreen() const{return false;}
    virtual bool isWindow() const{return false;}
    virtual void mouseClicked(QPoint) =0;

    inline void* thisBasePtr() {return (void*)static_cast<CursesBase*>(this);}

protected:
    inline explicit CursesBase(WINDOW* window =0) {_window=window;_winType=window?Screen:None;_dirty=true;}

    virtual void focusTaken() =0;
    virtual void focusGiven() =0;

    inline void create(CursesBase* par) {
        clear();

        if(par->isValid()) {
            if(isWindow() || par->isScreen()) {
                _window = newwin(geom().height(), geom().width(), geom().y(), geom().x());
                if(_window)
                    _winType = Window;
            } else {
                _window = derwin(par->hnd(), geom().height(), geom().width(), geom().y(), geom().x());
                if(_window)
                    _winType = Derived;
            }
            markDirty();
        }
    }

    inline void setPos(QPoint p) {
        switch(_winType) {
            case Screen:
                throw "Cannot move the screen.";

            case Window:
                mvwin(hnd(), p.y(), p.x());
                break;

            case Derived:
                mvderwin(hnd(), p.y(), p.x());
                break;

            case None:
                return;

        }
        notifyDirty();
    }

    inline void setSize(QSize s) {
        switch(_winType) {
            case Screen:
                throw "Cannot resize the screen.";

            case Window:
            case Derived:
                wresize(hnd(), s.height(), s.width());
                break;

            case None:
                return;

        }
        markDirty();
    }

    virtual void drawImpl() =0;

private:
    WinType _winType;

    WINDOW* _window;
    bool _dirty;

    inline virtual void draw() {if(_winType == Derived) {drawImpl();} else if(_dirty) {wclear(hnd());drawImpl();_dirty=false;}else touchwin(hnd());wnoutrefresh(hnd());}

    inline void clear() {
        if(_window) {
            delwin(_window);
            _window = 0;
        }
    }

    static CursesBase* _focusBase;
};

class CursesContainer : public CursesBase
{
    friend class CursesScreen;
public:

protected:
    inline CursesContainer(WINDOW* window =0) : CursesBase(window) {}
    virtual void drawChildren() =0;
    virtual void drawImpl() {}

private:
    inline void draw() {if(_winType == Derived) {drawImpl();} else if(_dirty) {wclear(hnd());drawImpl();_dirty=false;}else touchwin(hnd());wnoutrefresh(hnd());drawChildren();}

};

class CursesWindow : public CursesContainer
{
protected:
    inline CursesWindow(WINDOW* window =0) : CursesContainer(window) {}

    inline virtual bool isWindow() const{return false;}
};

class CursesScreen : public CursesWindow
{
public:
    inline virtual void updateParent(CursesBase*) {}
    inline bool isScreen() const{return true;}

protected:
    inline CursesScreen() : CursesWindow(initscr()) {
        start_color();

        mousemask(ALL_MOUSE_EVENTS, NULL);
        nodelay(hnd(), true);
        keypad(hnd(), true);
        meta(hnd(), true);
        curs_set(0);
    }

    inline void processMouseEvent(MEVENT& mEvent) {
        if(_cursor.isNull())
            curs_set(1);
        _cursor = QPoint(mEvent.x, mEvent.y);
        if(mEvent.bstate & BUTTON1_CLICKED)
            mouseClicked(_cursor);
        markDirty();
    }

    inline QSize checkSize() {
        return QSize(getmaxx(hnd()), getmaxy(hnd()));
    }

    inline void draw() {
        CursesContainer::draw();
        if(!_cursor.isNull())
            move(_cursor.y(), _cursor.x());

        doupdate();
    }

private:
    QPoint _cursor;
};

#define CURSES_IMPL  \
    inline void notifyDirty() {cursesDirtyMainWindow();} \
\
protected: \
    inline void posChanged() {CursesBase::setPos(pos());} \
    inline void parentChanged() {CursesBase::updateParent(((GUIWidget*)parentContainer())->internal<CursesBase>());} \
    inline void focusTaken() {if(!wattr().testFlag(GUIWidget::Focused))return;setWAttr(GUIWidget::Normal);markDirty();} \
    inline void focusGiven() {if(wattr().testFlag(GUIWidget::Focused))return;setWAttr(GUIWidget::Focused);markDirty();}


#define CURSES_CORE public:\
    virtual void* internalPtr() {return (void*)thisBasePtr();} \
    virtual void* handlePtr() {return (void*)hnd();} \
    inline QRect geom() const{return GUIWidget::geom();}

#define BASIC_CURSES_OBJECT CURSES_CORE CURSES_IMPL \
    inline void sizeChanged() {GUIWidget::sizeChanged();CursesBase::setSize(size());}

#define CURSES_OBJECT BASIC_CURSES_OBJECT  \
public: \
    inline void mouseClicked(QPoint) {emit clicked();focus();}

#define CURSES_CONTAINER_CORE CURSES_CORE


#define CURSES_CONTAINER CURSES_CONTAINER_CORE CURSES_IMPL \
    inline void sizeChanged() {GUIContainer::sizeChanged();CursesBase::setSize(size());}

#endif // CURSESWINDOW_H
