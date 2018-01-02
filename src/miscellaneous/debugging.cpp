// For license of this file, see <project-root-folder>/LICENSE.md.

#include "miscellaneous/debugging.h"

#include "miscellaneous/application.h"

#include <QDateTime>
#include <QDir>

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>

void Debugging::performLog(const QString& message, QtMsgType type, const char* file, const char* function, int line) {
  auto type_string = typeToString(type);

  // Write to console.
  if (file == nullptr || function == nullptr || line < 0) {
    std::cerr << "[" << APP_LOW_NAME << "] "
              << type_string.toUtf8().constData() << ": "
              << message.toUtf8().constData() << " ("
              << QDateTime::currentDateTime().toString(QSL("yyyy-MM-dd HH:mm:ss")).toUtf8().constData() << ")" << std::endl;
  }
  else {
    std::cerr << "[" << APP_LOW_NAME << "] "
              << message.toUtf8().constData() << " ("
              << QDateTime::currentDateTime().toString(QSL("yyyy-MM-dd HH:mm:ss")).toUtf8().constData() << ")" << std::endl
              << "  Type: " << type_string.toUtf8().constData() << std::endl
              << "  File: " << file << " (" << line << ")" << std::endl
              << "  Function: " << function << std::endl;
  }

  if (type == QtFatalMsg) {
    qApp->exit(EXIT_FAILURE);
  }
}

QString Debugging::typeToString(QtMsgType type) {
  switch (type) {
    case QtDebugMsg:
      return QSL("DEBUG");

    case QtWarningMsg:
      return QSL("WARNING");

    case QtCriticalMsg:
      return QSL("CRITICAL");

    case QtFatalMsg:
    default:
      return QSL("FATAL (terminating application)");
  }
}

void Debugging::debugHandler(QtMsgType type, const QMessageLogContext& placement, const QString& message) {
#ifndef QT_NO_DEBUG_OUTPUT
  performLog(message, type, placement.file, placement.function, placement.line);
#else
  Q_UNUSED(type)
  Q_UNUSED(placement)
  Q_UNUSED(message)
#endif
}
