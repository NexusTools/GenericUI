#-------------------------------------------------
#
# Project created by QtCreator 2013-11-21T16:37:22
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = test
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../core/release/ -lGenericUI
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../core/debug/ -lGenericUI
else:unix: LIBS += -L$$OUT_PWD/../core/ -lGenericUI

INCLUDEPATH += $$PWD/../core
DEPENDPATH += $$PWD/../core

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../qtgui/release/ -lGenericUIQtGui
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../qtgui/debug/ -lGenericUIQtGui
else:unix: LIBS += -L$$OUT_PWD/../qtgui/ -lGenericUIQtGui

INCLUDEPATH += $$PWD/../qtgui
DEPENDPATH += $$PWD/../qtgui

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../curses/release/ -lGenericNCursesUI
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../curses/debug/ -lGenericNCursesUI
else:unix: LIBS += -L$$OUT_PWD/../curses/ -lGenericNCursesUI

INCLUDEPATH += $$PWD/../curses
DEPENDPATH += $$PWD/../curses
