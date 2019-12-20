// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "common/miscellaneous/iofactory.h"

#include "common/exceptions/ioexception.h"
#include "definitions/definitions.h"

#include <QDataStream>
#include <QDebug>
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
    throw IOException(tr("Cannot open file '%1' for writing.").arg(QDir::toNativeSeparators(file_path)));
  }
}

QString IOFactory::writeToTempFile(const QByteArray& data) {
  QTemporaryFile tmp_file;

  tmp_file.setAutoRemove(false);
  tmp_file.setFileTemplate(QStandardPaths::writableLocation(QStandardPaths::TempLocation) +
                           QDir::separator() +
                           QSL("temp_file_XXXXXX.txt"));

  if (tmp_file.open()) {
    tmp_file.write(data);
    tmp_file.close();

    auto file_name = QDir::toNativeSeparators(tmp_file.fileName());

    if (!tmp_file.setPermissions(QFileDevice::Permission::ReadOwner | QFileDevice::Permission::WriteOwner)) {
      qWarning().noquote().nospace() << QSL("Failed to set permissions on temporary file '")
                                     << file_name
                                     << QSL("'.");
    }

    return file_name;
  }
  else {
    throw IOException(tr("Cannot open temporary file for writing."));
  }
}
