#ifndef CHANGESKI_GLOBAL_H
#define CHANGESKI_GLOBAL_H

#include <QtCore/qglobal.h>
#include <QWidget>
#include "QApplication"
#include "QMessageBox"
#include "windows.h"

#if defined(CHANGESKI_LIBRARY)
#  define CHANGESKI_EXPORT Q_DECL_EXPORT
#else
#  define CHANGESKI_EXPORT Q_DECL_IMPORT
#endif

#endif // CHANGESKI_GLOBAL_H
