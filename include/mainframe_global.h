#ifndef MAINFRAME_GLOBAL_H
#define MAINFRAME_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(RIBBONFRAME_LIBRARY)
#  define RIBBONFRAME_EXPORT Q_DECL_EXPORT
#else
#  define RIBBONFRAME_EXPORT
#endif

#endif // TMAINFRAME_GLOBAL_H
