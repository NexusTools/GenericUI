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

public:
    virtual ~CursesBase() {clear();if(_focusBase == this)_focusBase=0;}

    inline WINDOW* hnd() const{return _window;}

    inline bool isValid() const{return _window;}
    inline bool isDirty() const{return _dirty;}
    inline void markDirty() {_dirty=true;cursesDirtyMainWindow();}

    inline void focus() {
        if(_focusBase)
            _focusBase->focusTaken();
        _focusBase = static_cast<CursesBase*>(this);
        _focusBase->focusGiven();
    }

    virtual QRect geom() const =0;
    virtual bool isScreen() const{return false;}
    virtual bool isWindow() const{return false;}
    virtual void mouseClicked(QPoint) =0;

    inline void* thisBasePtr() {return (void*)static_cast<CursesBase*>(this);}

protected:
    inline explicit CursesBase(QSize size =QSize(1, 1)) {_window=newpad(size.height(), size.width());wbkgd(_window, COLOR_PAIR(1));if(!_window)throw "Unable to allocate pad";_dirty=true;}

    virtual void focusTaken() =0;
    virtual void focusGiven() =0;

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
        if(mEvent.bstate & BUTTON1_CLICKED)
            mouseClicked(_cursor);
        markDirty();
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

#define CURSES_IMPL  \
protected: \
    inline void posChanged() {cursesDirtyMainWindow();} \
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

#define CURSES_CONTAINER_CORE CURSES_CORE  \
protected: \
    inline void sizeChanged() {GUIContainer::sizeChanged();CursesBase::setSize(size());}

#define CURSES_CONTAINER CURSES_CONTAINER_CORE CURSES_IMPL

#define CURSES_WINDOW CURSES_CONTAINER \
    void visibilityChanged() { \
        if(isHidden()) \
            hideImpl(); \
        else \
            showImpl(); \
    }

#endif // CURSESWINDOW_H
