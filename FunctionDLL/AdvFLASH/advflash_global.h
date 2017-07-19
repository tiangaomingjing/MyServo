#ifndef ADVFLASH_GLOBAL_H
#define ADVFLASH_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(ADVFLASH_LIBRARY)
#  define ADVFLASHSHARED_EXPORT Q_DECL_EXPORT
#else
#  define ADVFLASHSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // ADVFLASH_GLOBAL_H