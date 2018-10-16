// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifndef DEBUGGING_H
#define DEBUGGING_H

#include <QtGlobal>

class TEXTOSAURUS_DLLSPEC Debugging {
  public:
    Debugging() = delete;
    ~Debugging() = delete;

    // Specifies format of output console messages.
    // NOTE: QT_NO_DEBUG_OUTPUT - disables debug outputs completely!!!
    static void debugHandler(QtMsgType type, const QMessageLogContext& placement, const QString& message);
    static void performLog(const QString& message, QtMsgType type, const char* file = nullptr,
                           const char* function = nullptr, int line = -1);
    static QString typeToString(QtMsgType type);
};

#endif // DEBUGGING_H
