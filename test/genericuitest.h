#ifndef GENERICUITEST_H
#define GENERICUITEST_H

#include <QDateTime>
#include <QObject>
#include <QTimer>
#include <QDebug>

#include <cursesaction.h>
#include <cursesmenubar.h>
#include <cursesmainwindow.h>
#include <curseslabel.h>

class GenericUITest : public QObject
{
    Q_OBJECT
public:
    inline explicit GenericUITest() : mainWindow("Generic UI Test") {
        CursesMenuBar* menuBar = new CursesMenuBar(&mainWindow);
        new CursesAction("_Test", menuBar);
        new CursesAction("_Help", menuBar);
        menuBar->fitToContent();
    }

private:

    CursesMainWindow mainWindow;
    
};

#endif // GENERICUITEST_H
