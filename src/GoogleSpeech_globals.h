#ifndef GOOGLESPEECH_GLOBAL_H
#define GOOGLESPEECH_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(GOOGLESPEECH_LIBRARY)
# define GOOGLESPEECHSHARED_EXPORT Q_DECL_EXPORT
#else
# define GOOGLESPEECHSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // GOOGLESPEECH_GLOBAL_H