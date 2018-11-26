// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "common/miscellaneous/cryptofactory.h"

#include "common/exceptions/ioexception.h"
#include "common/miscellaneous/iofactory.h"
#include "definitions/definitions.h"
#include "saurus/miscellaneous/application.h"

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

QStringList CryptoFactory::ciphers() {
  return {
           QSL("-aes-128-cbc"),
           QSL("-aes-128-ecb"),
           QSL("-aes-192-cbc"),
           QSL("-aes-192-ecb"),
           QSL("-aes-256-cbc"),
           QSL("-aes-256-ecb"),
           QSL("-base64"),
           QSL("-bf"),
           QSL("-bf-cbc"),
           QSL("-bf-cfb"),
           QSL("-bf-ecb"),
           QSL("-bf-ofb"),
           QSL("-camellia-128-cbc"),
           QSL("-camellia-128-ecb"),
           QSL("-camellia-192-cbc"),
           QSL("-camellia-192-ecb"),
           QSL("-camellia-256-cbc"),
           QSL("-camellia-256-ecb"),
           QSL("-cast"),
           QSL("-cast-cbc"),
           QSL("-cast5-cbc"),
           QSL("-cast5-cfb"),
           QSL("-cast5-ecb"),
           QSL("-cast5-ofb"),
           QSL("-des"),
           QSL("-des-cbc"),
           QSL("-des-cfb"),
           QSL("-des-ecb"),
           QSL("-des-ede"),
           QSL("-des-ede-cbc"),
           QSL("-des-ede-cfb"),
           QSL("-des-ede-ofb"),
           QSL("-des-ede3"),
           QSL("-des-ede3-cbc"),
           QSL("-des-ede3-cfb"),
           QSL("-des-ede3-ofb"),
           QSL("-des-ofb"),
           QSL("-des3"),
           QSL("-desx"),
           QSL("-idea"),
           QSL("-idea-cbc"),
           QSL("-idea-cfb"),
           QSL("-idea-ecb"),
           QSL("-idea-ofb"),
           QSL("-rc2"),
           QSL("-rc2-40-cbc"),
           QSL("-rc2-64-cbc"),
           QSL("-rc2-cbc"),
           QSL("-rc2-cfb"),
           QSL("-rc2-ecb"),
           QSL("-rc2-ofb"),
           QSL("-rc4"),
           QSL("-rc4-40"),
           QSL("-rc5"),
           QSL("-rc5-cbc"),
           QSL("-rc5-cfb"),
           QSL("-rc5-ecb"),
           QSL("-rc5-ofb"),
           QSL("-seed"),
           QSL("-seed-cbc"),
           QSL("-seed-cfb"),
           QSL("-seed-ecb"),
           QSL("-seed-ofb"),
           QSL("-zlib")
  };
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
  auto temp_input_file = IOFactory::writeToTempFile(data);

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

  QStringList args({
    "enc", qApp->settings()->value(GROUP(General), SETTING(General::EncryptionCipher)).toString(),
    "-salt", "-pass",
    "stdin", "-in", temp_input_file
  });

  if (qApp->settings()->value(GROUP(General), SETTING(General::EncryptionUsePbkdf2)).toBool()) {
    args.append(QSL("-pbkdf2"));
  }

  proc_openssl.start(program, args);

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
  auto temp_input_file = IOFactory::writeToTempFile(data);

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

  QStringList args({
    "enc", qApp->settings()->value(GROUP(General), SETTING(General::EncryptionCipher)).toString(),
    "-d", "-pass",
    "stdin", "-in", temp_input_file
  });

  if (qApp->settings()->value(GROUP(General), SETTING(General::EncryptionUsePbkdf2)).toBool()) {
    args.append(QSL("-pbkdf2"));
  }

  proc_openssl.start(program, args);

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
