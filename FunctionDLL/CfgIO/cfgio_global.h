#ifndef CFGIO_GLOBAL_H
#define CFGIO_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(CFGIO_LIBRARY)
#  define CFGIOSHARED_EXPORT Q_DECL_EXPORT
#else
#  define CFGIOSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // CFGIO_GLOBAL_H
