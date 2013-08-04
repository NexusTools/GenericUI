#ifndef GENERICUI_H
#define GENERICUI_H

#include "global.h"

#include <QObject>
#include <QDomNode>
#include <QRect>

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

    inline QPoint pos() const{return _geom.topLeft();}
    inline QSize size() const{return _geom.size();}
    inline QRect geom() const{return _geom;}

    virtual void processXML(QDomNode&);

    inline GUIWidget* parentWidget() const{return qobject_cast<GUIWidget*>(parent());}

    GUIMainWindow* mainWindow();
    GUIWindow* window();

protected:
    // Events
    virtual void focusIn() =0;
    virtual void focusOut() =0;
    virtual void keyTyped(char c) =0;

    virtual void posChanged() =0;
    virtual void sizeChanged() =0;
    virtual void geometryChanged() =0;
    virtual void visibilityChanged() =0;

    // Internals
    void setFocused(bool);
    void pushTypedKey(char c);

    inline void setPos(QPoint p) {_geom.setTopLeft(p);posChanged();geometryChanged();}
    inline void setSize(QSize s) {_geom.setSize(s);sizeChanged();geometryChanged();}
    inline void setGeometry(QRect r) {_geom=r;posChanged();geometryChanged();}

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
