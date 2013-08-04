#ifndef GENERICUITEST_H
#define GENERICUITEST_H

#include <QObject>
#include <QTimer>
#include <QDebug>

#include <cursesmainwindow.h>
#include <curseslabel.h>

class GenericUITest : public QObject
{
    Q_OBJECT
public:
    inline explicit GenericUITest() : mainWindow("Generic UI Test"), label1("Testing your love!", &mainWindow), label2("One day at a time...", &mainWindow) {
        label1.move(QPoint(5, 3));
        label2.move(QPoint(7, 5));
        move1 = true;
        move2 = true;

        animation.setInterval(50);
        connect(&animation, SIGNAL(timeout()), this, SLOT(move()));
        animation.start();
    }

public slots:
    inline void move() {
        label1.move(label1.pos().x() + (move1 ? 1 : -1), label1.pos().y());
        label2.move(label2.pos().x() + (move2 ? 1 : -1), label2.pos().y());
        if(label1.x() <= 0)
            move1 = true;
        else if(label1.x() + label1.width() >= mainWindow.width())
            move1 = false;
        if(label2.x() <= 0)
            move2 = true;
        else if(label2.x() + label2.width() >= mainWindow.width())
            move2 = false;
    }

private:
    QTimer animation;

    CursesMainWindow mainWindow;
    CursesLabel label1, label2;
    bool move1, move2;
    
};

#endif // GENERICUITEST_H
