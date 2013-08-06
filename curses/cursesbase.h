#ifndef CURSESWINDOW_H
#define CURSESWINDOW_H

#include <ncurses.h>
#include <guievent.h>

#include <QDebug>
#include <QRect>
#include <QList>

class GUIWidget;
class GUIMainWindow;

void cursesDirtyMainWindow();

class CursesBase
{
    friend class CursesContainer;
    friend class CursesScreen;

public:
    virtual ~CursesBase() {clear();if(_focusBase == this)_focusBase=0;}

    inline WINDOW* hnd() const{return _window;}

    inline bool isValid() const{return _window;}
    inline bool isDirty() const{return _dirty;}
    inline void markDirty() {_dirty=true;cursesDirtyMainWindow();}

    virtual QRect geom() const =0;
    virtual bool isScreen() const{return false;}
    virtual bool isWindow() const{return false;}

    inline void* thisBasePtr() {return (void*)static_cast<CursesBase*>(this);}

protected:
    inline explicit CursesBase(QSize size =QSize(1, 1)) {_window=newpad(size.height(), size.width());wbkgd(_window, COLOR_PAIR(1));if(!_window)throw "Unable to allocate pad";_dirty=true;}

    virtual bool processEvent(QObject*, QEvent*);
    virtual bool processEventFilter(QObject*, QEvent*) {return false;}

    inline void setSize(QSize s) {
        wresize(hnd(), s.height(), s.width());
        markDirty();
    }

    inline virtual void draw(QRect clip, QPoint off) {
        if(_dirty) {
            wclear(hnd());
            drawImpl();
            _dirty=false;
        } else
            touchwin(hnd());

        pnoutrefresh(hnd(), clip.y(), clip.x(), off.y(), off.x(), clip.height() + off.y(), clip.width() + off.x());
    }

    virtual void drawImpl() =0;

private:
    WINDOW* _window;
    bool _dirty;

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
    inline CursesContainer(QSize size =QSize(1,1)) : CursesBase(size) {}

    inline void draw(QRect clip, QPoint off) {
        CursesBase::draw(clip, off);
        drawChildren(clip, off);
    }
    inline void drawChild(CursesBase* child, QRect clip, QPoint off) {
        clip &= child->geom();
        if(clip.isEmpty())
            return;

        child->draw(QRect(clip.topLeft() - child->geom().topLeft(), clip.size()), off + child->geom().topLeft());
    }

    virtual void drawChildren(QRect clip, QPoint off) =0;
    virtual void drawImpl() {}

private:
};

class CursesWindow : public CursesContainer
{
protected:
    inline CursesWindow(QSize size =QSize(1,1)) : CursesContainer(size) {}

    inline virtual bool isWindow() const{return true;}

    virtual void showImpl();
    virtual void hideImpl();
};

class CursesEvent : public GUIEvent
{
public:
    enum CursesType {
        CursesTypeBase = GUIUserType,



        CursesUserType = CursesTypeBase + 50
    };
};

class CursesMouseEvent : public GUIEvent
{
public:
    enum CursesType {

    };
};

class CursesScreen : public CursesWindow
{
public:
    inline bool isScreen() const{return true;}
    inline void render() {
        draw(QRect(QPoint(0,0),geom().size()), QPoint(0,0));
    }

protected:
    inline CursesScreen(QSize s) : CursesWindow(s) {}

    inline void processMouseEvent(MEVENT& mEvent) {
        if(_cursor.isNull())
            curs_set(1);
        _cursor = QPoint(mEvent.x, mEvent.y);
        //if(mEvent.bstate & BUTTON1_CLICKED)
        //    mouseClicked(_cursor);
    }

    inline QSize checkSize() {
        return QSize(getmaxx(stdscr), getmaxy(stdscr));
    }

    inline void draw(QRect clip, QPoint off) {
        wnoutrefresh(stdscr);

        CursesContainer::draw(clip, off);
        if(!_cursor.isNull())
            move(_cursor.y(), _cursor.x());

        doupdate();
    }

private:
    QPoint _cursor;
};

#define CURSES_IMPL(FILTERCLASS) \
    virtual bool event(QEvent* ev) { \
        bool ret = processEvent(this, ev); \
        if(ret) \
            return ret; \
        return QObject::event(ev); \
    } \
        \
protected: \
    virtual bool eventFilter(QObject* obj, QEvent* ev) { \
        bool ret = processEventFilter(obj, ev); \
        if(ret) \
            return ret; \
        return FILTERCLASS::eventFilter(obj, ev); \
    }

#define CURSES_CORE public:\
    virtual void* internalPtr() {return (void*)thisBasePtr();} \
    virtual void* handlePtr() {return (void*)hnd();} \
    inline QRect geom() const{return GUIWidget::geom();}

#define BASIC_CURSES_OBJECT CURSES_CORE CURSES_IMPL(GUIWidget)

#define CURSES_OBJECT BASIC_CURSES_OBJECT  \
public:

#define CURSES_CONTAINER_CORE CURSES_CORE  \
protected:

#define CURSES_CONTAINER CURSES_CONTAINER_CORE CURSES_IMPL(GUIContainer)

#define CURSES_WINDOW CURSES_CONTAINER

#endif // CURSESWINDOW_H
