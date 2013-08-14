#-------------------------------------------------
#
# Project created by QtCreator 2013-08-03T21:16:49
#
#-------------------------------------------------

QT       += core xml
QT       -= gui

!greaterThan(QT_MAJOR_VERSION, 4): DEFINES += LEGACY_QT

TARGET = GenericUITest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

LIBS += -lncurses

SOURCES += main.cpp

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../curses/release/ -lGenericNCursesUI
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../curses/debug/ -lGenericNCursesUI
else:unix: LIBS += -L$$OUT_PWD/../curses/ -lGenericNCursesUI

INCLUDEPATH += $$PWD/../curses
DEPENDPATH += $$PWD/../curses

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../core/release/ -lGenericUI
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../core/debug/ -lGenericUI
else:unix: LIBS += -L$$OUT_PWD/../core/ -lGenericUI

INCLUDEPATH += $$PWD/../core
DEPENDPATH += $$PWD/../core

OTHER_FILES += \
    interface.xml

RESOURCES += \
    resources.qrc

HEADERS += \
    genericuitest.h
