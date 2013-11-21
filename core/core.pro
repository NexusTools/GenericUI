#-------------------------------------------------
#
# Project created by QtCreator 2013-08-03T20:38:33
#
#-------------------------------------------------

QT       -= gui
QT		 += xml

!greaterThan(QT_MAJOR_VERSION, 4): DEFINES += LEGACY_QT

TARGET = GenericUI
TEMPLATE = lib

DEFINES += GENERICUI_LIBRARY

SOURCES += \
    uiscreenthread.cpp \
    uiwidgetfactory.cpp \
    uiabstractscreen.cpp

HEADERS += \
    uiscreenthread.h \
    uiwidgetfactory.h \
    uiabstractscreen.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
