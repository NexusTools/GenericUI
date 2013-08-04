#include <QCoreApplication>

#include <unistd.h>
#include <cursesmainwindow.h>
#include <curseslabel.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    /*QDomDocument document;
    {
        QFile interface(":/interface.xml");
        interface.open(QFile::ReadOnly);
        QString errMsg;
        int column;
        int line;

        if(!document.setContent(&interface, &errMsg, &line, &column))
            qFatal("%s:%i:%i", qPrintable(errMsg), line, column);
    }*/

    //label.move(QPoint(5, 12));

    CursesMainWindow mainWindow("NCurses Generic UI Test");
    CursesLabel label("Testing your love!", &mainWindow);
    label.move(QPoint(5, 0));

    CursesLabel label2("One day at a time...", &mainWindow);
    label2.move(QPoint(7, 1));

    /*QDomNode xmlNode = document.firstChild();
    while(!xmlNode.isNull()) {
        if(xmlNode.nodeName() == "MainWindow")
            break;
        xmlNode = xmlNode.nextSibling();
    }
    if(!xmlNode.isNull())
        mainWindow.processXML(xmlNode);
    else
        throw "Missing MainWindow from xml definition.";*/
    
    return a.exec();
}
