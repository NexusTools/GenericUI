#include <QCoreApplication>

#include <unistd.h>
#include "genericuitest.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    GenericUITest genericUITest;
    
    return a.exec();
}
