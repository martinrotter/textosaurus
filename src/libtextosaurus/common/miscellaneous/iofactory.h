// For license of this file, see <project-root-folder>/LICENSE.md.

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

    static bool isFolderWritable(const QString& folder);

    // Returns system-wide folder according to type.
    static QString getSystemFolder(QStandardPaths::StandardLocation location);

    // Filters out shit characters from filename.
    static QString filterBadCharsFromFilename(const QString& name);
    static QByteArray readFileRawChunk(const QString& file_path, int length);

    // Returns contents of a file.
    // Throws exception when no such file exists.
    static QByteArray readFile(const QString& file_path);
    static void writeFile(const QString& file_path, const QByteArray& data);
    static QString writeToTempFile(const QByteArray& data);

    // Copies file, overwrites destination.
    static bool copyFile(const QString& source, const QString& destination);
};

#endif // IOFACTORY_H
