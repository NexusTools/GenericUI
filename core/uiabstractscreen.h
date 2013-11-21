#ifndef UISCREEN_H
#define UISCREEN_H

#include <QObject>

class UIScreenThread;
class UIWidgetFactory;

class UIAbstractScreen : public QObject
{
    Q_OBJECT

    enum Capability {
        ImageCapability,
        ColorCapability,
        MouseInputCapability,
        KeyboardInputCapability,
        IndirectRenderCapability,
        AcceleratedRenderCapability,
        FastRefreshCapability,
        OpenGLCapability
    };
    Q_DECLARE_FLAGS(Capabilities, Capability)

public:
    inline explicit UIAbstractScreen(Capabilities) {
    }
    
signals:
    void boundaryChanged();
    void viewportChanged();
    void visibilityChanged();
    void capabilitiesChanged();


private:

    
};

#endif // UISCREEN_H
