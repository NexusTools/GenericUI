#ifndef UIWIDGETFACTORY_H
#define UIWIDGETFACTORY_H

#include <QObject>

class UIWidget;

class UIWidgetFactory : public QObject
{
    Q_OBJECT
public:
    explicit UIWidgetFactory(QObject *parent = 0);
    
signals:
    
public slots:
    
};

#endif // UIWIDGETFACTORY_H
