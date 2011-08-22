#ifndef LIBPHANTOM_GLOBAL_H
#define LIBPHANTOM_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(LIBPHANTOM_LIBRARY)
#  define LIBPHANTOMSHARED_EXPORT Q_DECL_EXPORT
#else
#  define LIBPHANTOMSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // LIBPHANTOM_GLOBAL_H