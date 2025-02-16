#ifndef PTEMIDI_GLOBAL_H
#define PTEMIDI_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(PTEMIDI_LIBRARY)
#  define PTEMIDI_EXPORT Q_DECL_EXPORT
#else
#  define PTEMIDI_EXPORT Q_DECL_IMPORT
#endif

#endif // PTEMIDI_GLOBAL_H
