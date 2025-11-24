#ifndef SOUPLE_PDF_GLOBAL_H
#define SOUPLE_PDF_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(SOUPLE_PDF_LIBRARY)
#define SOUPLE_PDF_EXPORT Q_DECL_EXPORT
#else
#define SOUPLE_PDF_EXPORT Q_DECL_IMPORT
#endif

#endif // SOUPLE_PDF_GLOBAL_H
