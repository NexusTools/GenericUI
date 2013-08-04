#ifndef GENERICUI_H
#define GENERICUI_H

#include "global.h"

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
        Hidden,
        FloatRight,
        FloatCenter,
        ExpandHeight,
        ExpandWidth,
        Focused
    };
    Q_DECLARE_FLAGS(WAttrs, WAttr)

    inline void move(int x, int y) {setPos(QPoint(x, y));}
    inline void move(QPoint p) {setPos(p);}

    inline void resize(int w, int h) {setSize(QSize(w, h));}
    inline void resize(QSize s) {setSize(s);}

    inline int x() const{return _geom.x();}
    inline int y() const{return _geom.y();}
    inline int width() const{return _geom.width();}
    inline int height() const{return _geom.height();}

    inline QPoint pos() const{return _geom.topLeft();}
    inline QSize size() const{return _geom.size();}
    inline QRect geom() const{return _geom;}

    virtual void processXML(QDomNode&) {}
    void setParent(GUIContainer*);

    GUIContainer* parentContainer() const;
    GUIMainWindow* mainWindow();
    GUIWindow* window();

    bool event(QEvent *);

protected:
    inline GUIWidget(GUIContainer* parent =0) {setParent(parent);}

    // Events
    virtual void clicked() {}
    virtual void focusIn() {}
    virtual void focusOut() {}
    virtual void keyTyped(char) {}

    virtual void posChanged() {}
    virtual void sizeChanged() {}
    virtual void geometryChanged() {}
    virtual void visibilityChanged() {}

    virtual void parentChanged() {}

    // Internals
    void setFocused(bool);
    void pushTypedKey(char c);

    inline void setPos(QPoint p) {if(p==pos())return;_geom=QRect(p,size());posChanged();geometryChanged();}
    inline void setSize(QSize s) {if(s==size())return;_geom=QRect(pos(),s);sizeChanged();geometryChanged();}
    inline void setGeometry(QRect r) {_geom=r;posChanged();sizeChanged();geometryChanged();}

public slots:
    void show() {setVisible(true);}
    void hide() {setVisible(false);}
    void setVisible(bool vis) {if(_attr.testFlag(Hidden) != vis)return;if(vis)_attr^=Hidden;else{_attr&=Hidden;}visibilityChanged();}

signals:
    void focusGained();
    void focusLost();

    void shown();
    void hidden();

private:
    QRect _geom;
    WAttrs _attr;
};

#endif // GENERICUI_H
