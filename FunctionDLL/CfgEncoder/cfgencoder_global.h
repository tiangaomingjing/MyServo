#ifndef CFGENCODER_GLOBAL_H
#define CFGENCODER_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(CFGENCODER_LIBRARY)
#  define CFGENCODERSHARED_EXPORT Q_DECL_EXPORT
#else
#  define CFGENCODERSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // CFGENCODER_GLOBAL_H
