#ifndef GENERICUI_GLOBAL_H
#define GENERICUI_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(GENERICUI_LIBRARY)
#  define GENERICUISHARED_EXPORT Q_DECL_EXPORT
#else
#  define GENERICUISHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // GENERICUI_GLOBAL_H
