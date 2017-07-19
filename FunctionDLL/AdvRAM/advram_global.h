#ifndef ADVRAM_GLOBAL_H
#define ADVRAM_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(ADVRAM_LIBRARY)
#  define ADVRAMSHARED_EXPORT Q_DECL_EXPORT
#else
#  define ADVRAMSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // ADVRAM_GLOBAL_H
