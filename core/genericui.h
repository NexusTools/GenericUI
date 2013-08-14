#ifndef GENERICUI_H
#define GENERICUI_H

#include "global.h"
#include "guievent.h"

#include <QObject>
#include <QDomNode>
#include <QRect>

class GUIContainer;
class GUIMainWindow;
class GUIWindow;

class GENERICUISHARED_EXPORT GUIWidget : public QObject
{
    Q_OBJECT

    friend class GUIContainer;
public:
    enum WAttr {
        Normal = 0x0,

        FloatRight = 0x1,
        FloatCenter = 0x2,

        ExpandHeight = 0x4,
        ExpandWidth = 0x8,

        Focused = 0x10,
        Disabled = 0x20,
        Focusable = 0x80,
        Hidden = 0x40,

        DirtyLayout = 0x100,
        NoAutoResize = 0x200,
        LayoutDecor = 0x400
    };
    Q_DECLARE_FLAGS(WAttrs, WAttr)

    bool setWAttr(WAttr attr, bool on =true);
    inline WAttrs wattr() const{return _attr;}

    inline bool testAttr(WAttr wattr) {
        return _attr.testFlag(wattr);
    }

    inline bool isHidden() {
        return _attr.testFlag(Hidden);
    }

    inline bool isFocused() {
        return _attr.testFlag(Focused);
    }

    inline bool isFocusable() {
        return _attr.testFlag(Focusable) && !_attr.testFlag(Hidden) && !_attr.testFlag(Disabled);
    }

    inline bool isDisabled() {
        return _attr.testFlag(Disabled);
    }

    // Query Geometry
    int screenX() const;
    int screenY() const;
    inline int x() const{return _geom.x();}
    inline int y() const{return _geom.y();}

    inline int width() const{return _geom.width();}
    inline int height() const{return _geom.height();}

    inline QPoint pos() const{return _geom.topLeft();}
    inline QSize size() const{return _geom.size();}
    inline QRect geom() const{return _geom;}

    virtual QSize preferredSize() {return QSize(1,1);}


    inline virtual bool isWindow() const{return false;}
    virtual void processXML(QDomNode&) {}

    // Heirarchies
    GUIContainer* parentContainer() const;
    GUIMainWindow* mainWindow();
    GUIWindow* window();

    // Implementation Class Helpers
    template <class T>
    inline T* internal() {return static_cast<T*>(internalPtr());}
    template <class T>
    inline T* parentInternal() {GUIWidget* container=(GUIWidget*)parentContainer();if(!container)return 0;return static_cast<T*>(container->internalPtr());}
    template <class T>
    inline T* handle() const{return static_cast<T*>(handlePtr());}

    virtual void* internalPtr() =0;
    virtual void* handlePtr() const=0;

protected:
    inline GUIWidget(GUIContainer* parent =0) : _geom(0, 0, 1, 1) {setParent((QObject*)parent);_attr=Normal;}

    inline bool setWAttr(WAttrs attr) {if(_attr==attr)return false;_attr=attr;simEvent(GUIEvent::GUIWAttrChanged);return true;}

    // Internals
    void setPos(QPoint p);
    void setSize(QSize s);
    void setGeom(QRect r);

public slots:
    // Modify Geometry
    inline void fitToContent() {resize(preferredSize());}

    inline void move(int x, int y) {setPos(QPoint(x, y));}
    inline void move(QPoint p) {setPos(p);}

    inline void resize(int w, int h) {setSize(QSize(w, h));}
    inline void resize(QSize s) {setSize(s);}

    // Visibility
    void show(QPoint p){
        move(p);
        show();
    }
    void show() {setVisible(true);}
    void hide() {setVisible(false);}
    void setVisible(bool vis) {setHidden(!vis);}
    void setHidden(bool hdn);

    // State
    void enable() {setEnabled(true);}
    void disable() {setEnabled(false);}
    void setEnabled(bool en) {setDisabled(!en);}
    void focus() {setWAttr(Focused);}
    void setDisabled(bool dis);

    // Simulate
    inline void click() {simEvent(GUIEvent::GUIMouseClicked);}
    //inline void activate() {simEvent(GUIEvent::GUIActivated);}
    inline void simEvent(GUIEvent::GUIType t) {GUIEvent ev(t);event(&ev);}

    virtual bool event(QEvent *);

private:
    void stateChanged();

    void posChanged();
    void sizeChanged();
    void geomChanged();

    void focusGained();
    void focusLost();

    void visibilityChanged();
    void hidden();
    void shown();

signals:
    void clicked();

private:
    QRect _geom;
    WAttrs _attr;
};

#endif // GENERICUI_H
