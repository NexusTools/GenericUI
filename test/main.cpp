#include <QApplication>

#include <QDebug>

#include <unistd.h>
#include <stdio.h>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qDebug() << "Started";

    return a.exec();
}
