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
    friend class CursesMainWindow;
    friend class CursesContainer;
    friend class CursesScreen;

public:
    virtual ~CursesBase() {destroy();if(_focusBase == this)_focusBase=0;}

    inline WINDOW* hnd() const{return _window;}

    inline bool isValid() const{return _window;}
    inline bool isDirty() const{return _dirty;}
    virtual void markDirty();
    virtual void repaint();

    virtual QRect geom() const =0;
    virtual bool isScreen() const{return false;}
    virtual bool isWindow() const{return false;}

    inline void* thisBasePtr() {return (void*)static_cast<CursesBase*>(this);}

protected:
    inline explicit CursesBase(QSize size =QSize(1, 1)) {_window=newpad(size.height(), size.width());wbkgd(_window, COLOR_PAIR(1));if(!_window)throw "Unable to allocate pad";_dirty=true;}
    virtual bool processEvent(QEvent*);

    inline void setSize(QSize s) {
        wresize(hnd(), s.height(), s.width());
        markDirty();
    }

    virtual void draw(QRect clip, QPoint off) {
        if(_dirty) {
            wclear(hnd());
            drawImpl();
            _dirty=false;
        }

        pnoutrefresh(hnd(), clip.y(), clip.x(), off.y(), off.x(), clip.height() + off.y(), clip.width() + off.x());
    }

    virtual void drawImpl() =0;
    virtual GUIWidget* widget() =0;

private:
    WINDOW* _window;
    bool _dirty;

    inline void destroy() {
        if(_window) {
            delwin(_window);
            _window = 0;
        }
    }

    static CursesBase* _focusBase;
};

class CursesBaseAction : public CursesBase
{
protected:
    virtual bool processEvent(QEvent*);
};

class CursesContainer : public CursesBase
{
    friend class CursesScreen;
public:

protected:
    inline CursesContainer(QSize size =QSize(1,1)) : CursesBase(size) {}
    virtual bool processEvent(QEvent*);

    virtual void draw(QRect clip, QPoint off) {
        CursesBase::draw(clip, off);
        drawChildren(clip, off);
    }
    inline void drawChild(CursesBase* child, QRect clip, QPoint off) {
        QRect nclip = clip & child->geom();
        if(nclip.isEmpty()) {
            qDebug() << "Child obstructed" << child->geom() << nclip << clip << off;
            return;
        }

        qDebug() << "Drawing child" << child << clip << off;
        child->draw(QRect(nclip.topLeft() - child->geom().topLeft(), nclip.size()), off + child->geom().topLeft());
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

    virtual bool processEvent(QEvent *ev);
    virtual void showImpl();
    virtual void hideImpl();
};

class CursesScreen : public CursesWindow
{
public:
    inline bool isScreen() const{return true;}
    inline void render() {
        qDebug() << "Rendering" << this;
        draw(QRect(QPoint(0,0),geom().size()), QPoint(0,0));
    }

protected:
    inline CursesScreen(QSize s) : CursesWindow(s) {}

    virtual bool processEvent(QEvent *ev) {
        return CursesContainer::processEvent(ev);
    }

    void processMouseEvent(MEVENT& mEvent);
    inline QSize checkSize() {
        return QSize(getmaxx(stdscr), getmaxy(stdscr));
    }

    virtual void draw(QRect clip, QPoint off) {
        wnoutrefresh(stdscr);

        CursesContainer::draw(clip, off);
        if(!_cursor.isNull())
            move(_cursor.y(), _cursor.x());

        doupdate();
    }

private:
    QPoint _cursor;
};

#define CURSES_IMPL(EVENTCLASS) \
public: \
    virtual bool event(QEvent* ev) { \
        bool ret = processEvent(ev); \
        if(ret) \
            return ret; \
        return EVENTCLASS::event(ev); \
    }

#define CURSES_CORE public:\
    virtual void* internalPtr() {return (void*)thisBasePtr();} \
    virtual void* handlePtr() const{return (void*)hnd();} \
    inline QRect geom() const{return GUIWidget::geom();} \
\
protected: \
    virtual GUIWidget* widget() {return this;}

#define BASIC_CURSES_OBJECT CURSES_CORE CURSES_IMPL(GUIWidget)

#define CURSES_OBJECT BASIC_CURSES_OBJECT

#define CURSES_CONTAINER_CORE CURSES_CORE

#define CURSES_CONTAINER CURSES_CONTAINER_CORE CURSES_IMPL(GUIContainer)

#define CURSES_WINDOW CURSES_CONTAINER

#endif // CURSESWINDOW_H
