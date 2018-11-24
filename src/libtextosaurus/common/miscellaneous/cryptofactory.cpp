// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "common/miscellaneous/cryptofactory.h"

#include "common/exceptions/ioexception.h"
#include "common/miscellaneous/iofactory.h"
#include "definitions/definitions.h"

#include <QCoreApplication>
#include <QCryptographicHash>
#include <QDebug>
#include <QDir>
#include <QProcess>

bool CryptoFactory::isEncrypted(const QByteArray& data) {
  return data.left(8) == QSL("Salted__");
}

QString CryptoFactory::openSslVersion() {
  QProcess proc_openssl(qApp);
  QString program = openSslBinaryPath();

  proc_openssl.start(program, { QSL("version") });
  proc_openssl.waitForFinished(2000);

  QByteArray version = proc_openssl.readAllStandardOutput();

  return version.isEmpty() ? QObject::tr("not installed") : version;
}

QString CryptoFactory::openSslBinaryPath() {
#if defined (Q_OS_WIN)
  QString program = QDir::toNativeSeparators(qApp->applicationDirPath()) + QDir::separator() + QL1S("openssl.exe");
#else
  QString program(QSL("openssl"));
#endif

  return program;
}

QByteArray CryptoFactory::encryptData(const QString& password, const QByteArray& data) {
  if (password.isEmpty()) {
    throw ApplicationException(QObject::tr("cannot encrypt data with empty password"));
  }

  // Save unencrypted data to temp file first.
  auto temp_input_file = QDir::toNativeSeparators(IOFactory::writeToTempFile(data));

  // Run OpenSSL binary and encrypt the file.
  // Password is passed to standard input.
  // Encrypted data is read from standard output.
  QProcess proc_openssl(qApp);
  QString program = openSslBinaryPath();

  proc_openssl.connect(&proc_openssl, &QProcess::started, [password, &proc_openssl] {
    // Pass password via stdin.
    proc_openssl.write(password.toUtf8());
    proc_openssl.closeWriteChannel();
  });

  proc_openssl.start(program, {
    "enc", "-aes-256-cbc", "-salt", "-pass", "stdin", "-in", temp_input_file
  });

  if (!proc_openssl.waitForFinished(10000)) {
    QFile::remove(temp_input_file);
    throw ApplicationException(QObject::tr("encryption failed with error '%1'").arg(proc_openssl.errorString()));
  }

  switch (proc_openssl.error()) {
    case QProcess::ProcessError::Crashed:
      throw ApplicationException(QObject::tr("\"openssl\" utility crashed"));
      break;

    case QProcess::ProcessError::FailedToStart:
      throw ApplicationException(QObject::tr("\"openssl\" utility was not found"));
      break;

    case QProcess::ProcessError::ReadError:
      throw ApplicationException(QObject::tr("\"openssl\" utility was not able to read input"));
      break;

    case QProcess::ProcessError::Timedout:
      throw ApplicationException(QObject::tr("\"openssl\" utility timed out"));
      break;

    case QProcess::ProcessError::WriteError:
      throw ApplicationException(QObject::tr("\"openssl\" utility cannot write output"));
      break;

    case QProcess::ProcessError::UnknownError:
      break;
  }

  if (proc_openssl.exitCode() != EXIT_SUCCESS) {
    // Failure.
    QFile::remove(temp_input_file);
    throw ApplicationException(QObject::tr("encryption failed with error '%1'").arg(QString::fromUtf8(proc_openssl.readAllStandardError())));
  }

  // Collect encrypted data from OpenSSL output.
  auto output = proc_openssl.readAllStandardOutput();

  // Cleanup.
  QFile::remove(temp_input_file);

  return output;
}

QByteArray CryptoFactory::decryptData(const QString& password, const QByteArray& data) {
  if (password.isEmpty()) {
    throw ApplicationException(QObject::tr("cannot decrypt data with empty password"));
  }

  // Save encrypted data to temp file first.
  auto temp_input_file = QDir::toNativeSeparators(IOFactory::writeToTempFile(data));

  // Run OpenSSL binary and encrypt the file.
  // Password is passed to standard input.
  // Encrypted data is read from standard output.
  QProcess proc_openssl(qApp);
  QString program = openSslBinaryPath();

  proc_openssl.connect(&proc_openssl, &QProcess::started, [password, &proc_openssl] {
    // Pass password via stdin.
    proc_openssl.write(password.toUtf8());
    proc_openssl.closeWriteChannel();
  });

  proc_openssl.start(program, {
    "enc", "-aes-256-cbc", "-d", "-pass", "stdin", "-in", temp_input_file
  });

  if (!proc_openssl.waitForFinished(10000)) {
    QFile::remove(temp_input_file);
    throw ApplicationException(QObject::tr("decryption failed with error '%1'").arg(proc_openssl.errorString()));
  }

  switch (proc_openssl.error()) {
    case QProcess::ProcessError::Crashed:
      throw ApplicationException(QObject::tr("\"openssl\" utility crashed"));
      break;

    case QProcess::ProcessError::FailedToStart:
      throw ApplicationException(QObject::tr("\"openssl\" utility was not found"));
      break;

    case QProcess::ProcessError::ReadError:
      throw ApplicationException(QObject::tr("\"openssl\" utility was not able to read input"));
      break;

    case QProcess::ProcessError::Timedout:
      throw ApplicationException(QObject::tr("\"openssl\" utility timed out"));
      break;

    case QProcess::ProcessError::WriteError:
      throw ApplicationException(QObject::tr("\"openssl\" utility cannot write output"));
      break;

    case QProcess::ProcessError::UnknownError:
      break;
  }

  if (proc_openssl.exitCode() != EXIT_SUCCESS) {
    // Failure.
    QFile::remove(temp_input_file);
    throw ApplicationException(QObject::tr("decryption failed with error '%1'").arg(QString::fromUtf8(proc_openssl.readAllStandardError())));
  }

  // Collect decrypted data from OpenSSL output.
  auto output = proc_openssl.readAllStandardOutput();

  // Cleanup.
  QFile::remove(temp_input_file);

  return output;
}

void CryptoFactory::testPassword(const QString& password, const QByteArray& data) {
  decryptData(password, data);
}
