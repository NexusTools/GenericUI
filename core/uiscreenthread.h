#ifndef UISCREENTHREAD_H
#define UISCREENTHREAD_H

#include <QThread>

class UIScreenThread : public QThread
{
    Q_OBJECT
public:
    explicit UIScreenThread(QObject *parent = 0);
    
signals:
    
public slots:
    
};

#endif // UISCREENTHREAD_H
