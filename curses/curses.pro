#-------------------------------------------------
#
# Project created by QtCreator 2013-08-03T20:52:18
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = GenericNCursesUI
CONFIG   += console
CONFIG   -= app_bundle

DEFINES += GENERICUI_CURSES

TEMPLATE = lib


SOURCES += \
    cursesmainwindow.cpp

HEADERS += \
    global.h \
    curseslabel.h \
    cursesmainwindow.h

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../core/release/ -lGenericUI
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../core/debug/ -lGenericUI
else:unix: LIBS += -L$$OUT_PWD/../core/ -lGenericUI

INCLUDEPATH += $$PWD/../core
DEPENDPATH += $$PWD/../core
