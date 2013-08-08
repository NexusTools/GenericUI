#-------------------------------------------------
#
# Project created by QtCreator 2013-08-03T20:52:18
#
#-------------------------------------------------

QT       += core xml

QT       -= gui

TARGET = GenericNCursesUI
CONFIG   += console
CONFIG   -= app_bundle

DEFINES += GENERICUI_CURSES

TEMPLATE = lib

LIBS += -lncurses


SOURCES += \
    cursesmainwindow.cpp \
    cursesbase.cpp \
    cursesaction.cpp \
    cursesbutton.cpp \
    cursesedit.cpp

HEADERS += \
    global.h \
    curseslabel.h \
    cursesmainwindow.h \
    cursesbase.h \
    cursesaction.h \
    cursesmenubar.h \
    cursesmenu.h \
    cursesmenuseparator.h \
    cursesdialog.h \
    cursesbutton.h \
    cursescontainer.h \
    cursesbuttonbox.h \
    curseslineedit.h

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../core/release/ -lGenericUI
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../core/debug/ -lGenericUI
else:unix: LIBS += -L$$OUT_PWD/../core/ -lGenericUI

INCLUDEPATH += $$PWD/../core
DEPENDPATH += $$PWD/../core

unix:!symbian {
	maemo5 {
		target.path = /opt/usr/lib
	} else {
		target.path = /usr/lib
	}
	INSTALLS += target
}
