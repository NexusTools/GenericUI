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
public:
    enum WAttr {
        Normal,

        FloatRight,
        FloatCenter,

        ExpandHeight,
        ExpandWidth,

        Focused,
        Disabled,
        Hidden
    };
    Q_DECLARE_FLAGS(WAttrs, WAttr)

    inline WAttrs wattr() const{return _attr;}

    inline bool testAttr(WAttr wattr) {
        return _attr.testFlag(wattr);
    }

    inline bool isHidden() {
        return _attr.testFlag(Hidden);
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

    // Preferred Size
    virtual QSize preferredSize() {return QSize(1,1);}

    inline virtual bool isWindow() const{return false;}
    virtual void processXML(QDomNode&) {}
    void setParent(GUIContainer*);

    // Heirarchies
    GUIContainer* parentContainer() const;
    GUIMainWindow* mainWindow();
    GUIWindow* window();

    // Implementation Class Helpers
    template <class T>
    inline T* internal() {return static_cast<T*>(internalPtr());}
    template <class T>
    inline T* handle() {return static_cast<T*>(handlePtr());}

    virtual void* internalPtr() =0;
    virtual void* handlePtr() =0;

    virtual bool event(QEvent *);

protected:
    inline GUIWidget(GUIContainer* parent =0) : _geom(0, 0, 1, 1) {setParent(parent);_attr=Normal;}

    inline void setWAttr(WAttrs attr) {_attr=attr;pushEvent(GUIEvent::GUIWAttrChanged);}
    inline void pushEvent(GUIEvent::GUIType t) {GUIEvent ev(t);event(&ev);}

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
    void setDisabled(bool dis);

signals:
    void stateChanged();

    void posChanged();
    void sizeChanged();
    void geomChanged();

    void focusGained();
    void focusLost();

    void visibilityChanged();
    void hidden();
    void shown();

    void activated();
    void clicked();

private:
    QRect _geom;
    WAttrs _attr;
};

#endif // GENERICUI_H
