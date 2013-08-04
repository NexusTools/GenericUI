#ifndef CURSESWINDOW_H
#define CURSESWINDOW_H

#include <ncurses.h>

#include <QDebug>
#include <QRect>
#include <QList>

class GUIMainWindow;

void cursesDirtyMainWindow(GUIMainWindow*);

class CursesBase
{
    friend class CursesContainer;

    enum WinType {
        None,

        Screen,
        Window,
        Derived
    };

public:
    virtual ~CursesBase() {clear();}

    inline WINDOW* hnd() const{return _window;}

    inline void render(CursesBase* par) {if(!isValid())create(par);draw();}
    inline bool isValid() const{return _window;}
    inline bool isDirty() const{return _dirty;}
    inline void markDirty() {_dirty=true;notifyDirty();}
    virtual void notifyDirty() =0;

    virtual QRect geom() const =0;
    inline virtual void updateParent(CursesBase* par) {
        if(_window)
            CursesBase::clear();
        if(par)
            create(par);
    }
    inline virtual bool isScreen() const{return false;}

protected:
    inline explicit CursesBase(WINDOW* window =0) {_window=window;_winType=window?Screen:None;_dirty=true;}

    inline void create(CursesBase* par) {
        clear();

        if(par->isValid()) {
            if(par->isScreen()) {
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

    inline virtual void drawImpl() {}

private:
    WinType _winType;

    WINDOW* _window;
    bool _dirty;

    inline virtual void draw() {if(_dirty) {drawImpl();_dirty=false;}wnoutrefresh(hnd());}

    inline void clear() {
        if(_window) {
            delwin(_window);
            _window = 0;
        }
    }
};

class CursesContainer : public CursesBase
{
    friend class CursesScreen;
public:

protected:
    inline CursesContainer(WINDOW* window =0) : CursesBase(window) {}
    virtual void drawChildren() =0;

private:
    inline void draw() {if(_dirty) {drawImpl();_dirty=false;}else touchwin(hnd());wnoutrefresh(hnd());drawChildren();}
};

class CursesScreen : public CursesContainer
{
public:
    inline virtual void updateParent(CursesBase*) {}
    inline bool isScreen() const{return true;}

protected:
    inline CursesScreen() : CursesContainer(initscr()) {
        start_color();

        nodelay(hnd(), true);
        keypad(hnd(), true);
        meta(hnd(), true);
    }

    inline QSize checkSize() {
        return QSize(getmaxx(hnd()), getmaxy(hnd()));
    }

    inline void draw() {CursesContainer::draw();doupdate();}

};

#endif // CURSESWINDOW_H
