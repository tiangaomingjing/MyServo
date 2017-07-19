#ifndef CFGLIMIT_GLOBAL_H
#define CFGLIMIT_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(CFGLIMIT_LIBRARY)
#  define CFGLIMITSHARED_EXPORT Q_DECL_EXPORT
#else
#  define CFGLIMITSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // CFGLIMIT_GLOBAL_H