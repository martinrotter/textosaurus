// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifndef IOFACTORY_H
#define IOFACTORY_H

#include "definitions/definitions.h"

#include <QCoreApplication>
#include <QStandardPaths>

class TEXTOSAURUS_DLLSPEC IOFactory {
  Q_DECLARE_TR_FUNCTIONS(IOFactory)

  public:
    IOFactory() = delete;
    ~IOFactory() = delete;
    IOFactory& operator=(const IOFactory&) = delete;
    static bool isFolderWritable(const QString& folder);
    static QByteArray readFileRawChunk(const QString& file_path, int length);

    // Returns contents of a file.
    // Throws exception when no such file exists.
    static QByteArray readFile(const QString& file_path);
    static void writeFile(const QString& file_path, const QByteArray& data);
    static QString writeToTempFile(const QByteArray& data);
};

#endif // IOFACTORY_H
