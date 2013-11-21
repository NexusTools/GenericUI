#-------------------------------------------------
#
# Project created by QtCreator 2013-11-21T16:35:55
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GenericUIQtGui
CONFIG   -= app_bundle

TEMPLATE = lib

SOURCES +=

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../core/release/ -lGenericUI
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../core/debug/ -lGenericUI
else:unix: LIBS += -L$$OUT_PWD/../core/ -lGenericUI

INCLUDEPATH += $$PWD/../core
DEPENDPATH += $$PWD/../core
