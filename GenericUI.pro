TEMPLATE = subdirs

SUBDIRS += core \
    qtgui \
    test

packagesExist(ncurses):SUBDIRS += curses
