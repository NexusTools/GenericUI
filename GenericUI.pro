TEMPLATE = subdirs

SUBDIRS += core

packagesExist(ncurses):SUBDIRS += curses test
