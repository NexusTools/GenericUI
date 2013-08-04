#-------------------------------------------------
#
# Project created by QtCreator 2013-08-03T20:38:33
#
#-------------------------------------------------

QT       -= gui
QT		 += xml

TARGET = GenericUI
TEMPLATE = lib

DEFINES += GENERICUI_LIBRARY

SOURCES +=

HEADERS += genericui.h \
    global.h \
    guilabel.h \
    guicontainer.h \
    guiwindow.h \
    guimainwindow.h \
    guidialog.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
