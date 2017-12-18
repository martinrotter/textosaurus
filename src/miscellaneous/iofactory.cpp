// For license of this file, see <project-root-folder>/LICENSE.md.

#include "miscellaneous/iofactory.h"

#include "definitions/definitions.h"
#include "exceptions/ioexception.h"
#include "miscellaneous/application.h"

#include <QDataStream>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QObject>
#include <QTemporaryFile>

bool IOFactory::isFolderWritable(const QString& folder) {
  QString real_file = folder;

  if (!real_file.endsWith(QDir::separator())) {
    real_file += QDir::separator();
  }

  real_file += "test-permissions-file";
  return QTemporaryFile(real_file).open();
}

QString IOFactory::getSystemFolder(QStandardPaths::StandardLocation location) {
  return QStandardPaths::writableLocation(location);
}

QString IOFactory::filterBadCharsFromFilename(const QString& name) {
  QString value = name;

  value.replace(QL1C('/'), QL1C('-'));
  value.remove(QL1C('\\'));
  value.remove(QL1C(':'));
  value.remove(QL1C('*'));
  value.remove(QL1C('?'));
  value.remove(QL1C('"'));
  value.remove(QL1C('<'));
  value.remove(QL1C('>'));
  value.remove(QL1C('|'));
  return value;
}

QByteArray IOFactory::readFileRawChunk(const QString& file_path, int length) {
  QFile input_file(file_path);

  if (input_file.open(QIODevice::ReadOnly)) {
    QByteArray chunk = input_file.read(qint64(length));

    input_file.close();

    return chunk;
  }
  else {
    return QByteArray();
  }
}

QByteArray IOFactory::readFile(const QString& file_path) {
  QFile input_file(file_path);
  QByteArray input_data;

  if (input_file.open(QIODevice::ReadOnly)) {
    input_data = input_file.readAll();
    input_file.close();
    return input_data;
  }
  else {
    throw IOException(tr("Cannot open file '%1' for reading.").arg(QDir::toNativeSeparators(file_path)));
  }
}

void IOFactory::writeFile(const QString& file_path, const QByteArray& data) {
  QFile input_file(file_path);

  if (input_file.open(QIODevice::WriteOnly)) {
    input_file.write(data);
    input_file.close();
  }
  else {
    throw IOException(tr("Cannot open file '%1' for writting.").arg(QDir::toNativeSeparators(file_path)));
  }
}

QString IOFactory::writeToTempFile(const QByteArray& data) {
  QTemporaryFile tmp_file;

  tmp_file.setAutoRemove(false);
  tmp_file.setFileTemplate(qApp->tempFolder() + QDir::separator() + QSL("tool_output_XXXXXX.txt"));

  if (tmp_file.open()) {
    tmp_file.write(data);
    tmp_file.close();

    return tmp_file.fileName();
  }
  else {
    throw IOException(tr("Cannot open temporary file for writting."));
  }
}

bool IOFactory::copyFile(const QString& source, const QString& destination) {
  if (QFile::exists(destination)) {
    if (!QFile::remove(destination)) {
      return false;
    }
  }

  return QFile::copy(source, destination);
}
