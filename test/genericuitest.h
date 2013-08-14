#ifndef GENERICUITEST_H
#define GENERICUITEST_H

#include <QDateTime>
#include <QObject>
#include <QTimer>
#include <QDebug>

#include <cursesdialog.h>
#include <cursesaction.h>
#include <cursesmenubar.h>
#include <cursesmainwindow.h>
//#include <cursesscrollbar.h>
#include <curseslabel.h>

class GenericUITest : public QObject
{
    Q_OBJECT
public:
    inline explicit GenericUITest() : mainWindow("Generic UI Test") {
        CursesMenuBar* menuBar = new CursesMenuBar(&mainWindow);

        CursesAction* act = new CursesAction("He_llo", menuBar);
        connect(act, SIGNAL(activated()), this, SLOT(testDiag()));
    }

public slots:
    void testDiag() {
        CursesDialog::alert("Pumpkin Pie!");
    }

private:

    CursesMainWindow mainWindow;
    
};

#endif // GENERICUITEST_H
