#ifndef GENERICUI_CURSES_GLOBAL_H
#define GENERICUI_CURSES_GLOBAL_H


#include <QtCore/qglobal.h>

#if defined(GENERICUI_CURSES)
#  define GENERICUI_CURSES_EXPORT Q_DECL_EXPORT
#else
#  define GENERICUI_CURSES_EXPORT Q_DECL_IMPORT
#endif

#endif // GLOBAL_H
