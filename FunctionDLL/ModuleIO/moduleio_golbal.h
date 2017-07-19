#ifndef MODULEIO_GOLBAL_H
#define MODULEIO_GOLBAL_H

#include <QtCore/qglobal.h>

#if defined(MODULEIO_LIBRARY)
#  define MODULEIOSHARED_EXPORT Q_DECL_EXPORT
#else
#  define MODULEIOSHARED_EXPORT Q_DECL_IMPORT
#endif


#endif // MODULEIO_GOLBAL

