#ifndef CFGCURRENTLOOP_GLOBAL_H
#define CFGCURRENTLOOP_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(CFGCURRENTLOOP_LIBRARY)
#  define CFGCURRENTLOOPSHARED_EXPORT Q_DECL_EXPORT
#else
#  define CFGCURRENTLOOPSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // CFGCURRENTLOOP_GLOBAL_H
