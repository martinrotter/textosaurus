// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "common/miscellaneous/cryptofactory.h"

#include "common/exceptions/ioexception.h"
#include "common/miscellaneous/iofactory.h"
#include "definitions/definitions.h"

#include <QCoreApplication>
#include <QCryptographicHash>
#include <QDir>
#include <QProcess>

bool CryptoFactory::isEncrypted(const QByteArray& data) {
  return data.left(8) == QSL("Salted__");
}

QByteArray CryptoFactory::encryptData(const QString& password, const QByteArray& data) {
  if (data.isEmpty()) {
    return QByteArray();
  }

  if (password.isEmpty()) {
    throw ApplicationException(QObject::tr("cannot encrypt file with empty password"));
  }

  // Save unencrypted data to temp file first.
  auto temp_input_file = QDir::toNativeSeparators(IOFactory::writeToTempFile(data));

  // Run OpenSSL binary and encrypt the file.
  // Password is passed to standard input.
  // Encrypted data is read from standard output.
  QProcess proc_openssl(qApp);

  proc_openssl.setProcessEnvironment(QProcessEnvironment::systemEnvironment());
  proc_openssl.connect(&proc_openssl, &QProcess::started, [password, &proc_openssl] {
    proc_openssl.write(password.toUtf8());
    proc_openssl.closeWriteChannel();
  });

#if defined (Q_OS_WIN)
  QString program = QDir::toNativeSeparators(qApp->applicationDirPath()) + QDir::separator() + QL1S("openssl.exe");
#else
  QString program(QSL("openssl"));
#endif

  proc_openssl.start(program, {
    "enc", "-aes-256-cbc", "-salt", "-pass", "stdin", "-in", temp_input_file
  });

  if (!proc_openssl.waitForFinished()) {
    QFile::remove(temp_input_file);
    throw ApplicationException(QObject::tr("encryption failed"));
  }

  if (proc_openssl.exitCode() != EXIT_SUCCESS) {
    // Failure.
    QFile::remove(temp_input_file);
    throw ApplicationException(QObject::tr("encryption failed with error: '%1'").arg(QString::fromUtf8(proc_openssl.readAllStandardError())));
  }

  // Collect encrypted data from OpenSSL output.
  auto output = proc_openssl.readAllStandardOutput();

  // Cleanup.
  QFile::remove(temp_input_file);

  return output;
}

QByteArray CryptoFactory::decryptData(const QString& password, const QByteArray& data) {
  // openssl enc -aes-256-cbc -d -pass stdin -in text.enc
  // pass password as stdin
  // decrypted is in stdout
  // non-zero return value -> pad password or other error

  if (password.isEmpty() || data.isEmpty()) {
    throw ApplicationException(QObject::tr("password cannot be empty"));
  }

  // Save encrypted data to temp file first.
  auto temp_input_file = QDir::toNativeSeparators(IOFactory::writeToTempFile(data));

  // Run OpenSSL binary and encrypt the file.
  // Password is passed to standard input.
  // Encrypted data is read from standard output.
  QProcess proc_openssl(qApp);

  proc_openssl.setProcessEnvironment(QProcessEnvironment::systemEnvironment());
  proc_openssl.connect(&proc_openssl, &QProcess::started, [password, &proc_openssl] {
    proc_openssl.write(password.toUtf8());
    proc_openssl.closeWriteChannel();
  });

#if defined (Q_OS_WIN)
  QString program = QDir::toNativeSeparators(qApp->applicationDirPath()) + QDir::separator() + QL1S("openssl.exe");
#else
  QString program(QSL("openssl"));
#endif

  proc_openssl.start(program, {
    "enc", "-aes-256-cbc", "-d", "-pass", "stdin", "-in", temp_input_file
  });

  if (!proc_openssl.waitForFinished()) {
    QFile::remove(temp_input_file);
    throw ApplicationException(QObject::tr("decryption failed"));
  }

  if (proc_openssl.exitCode() != EXIT_SUCCESS) {
    // Failure.
    QFile::remove(temp_input_file);
    throw ApplicationException(QObject::tr("decryption failed with error: '%1'").arg(QString::fromUtf8(proc_openssl.readAllStandardError())));
  }

  // Collect decrypted data from OpenSSL output.
  auto output = proc_openssl.readAllStandardOutput();

  // Cleanup.
  QFile::remove(temp_input_file);

  return output;
}

bool CryptoFactory::isPasswordCorrect(const QString& password, const QByteArray& data) {
  try {
    decryptData(password, data);
    return true;
  }
  catch (...) {
    return false;
  }
}
