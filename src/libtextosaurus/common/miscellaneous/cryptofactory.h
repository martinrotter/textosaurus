// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifndef CRYPTOFACTORY_H
#define CRYPTOFACTORY_H

#include <QFile>

// Simple encryption for byte-arrays or files based on OpenSSL.
// Textosaurus is not linking directly against OpenSSL libraries,
// but "openssl" binary is called instead, particularly its "enc"
// functionality is used.
class CryptoFactory {
  public:
    CryptoFactory() = delete;
    ~CryptoFactory() = delete;

    // Encrypts data with given password.
    static QByteArray encryptData(const QString& password, const QByteArray& data);

    // Decrypts data with given password.
    // Throws exception if password is invalid or other
    // problem appears.
    static QByteArray decryptData(const QString& password, const QByteArray& data);

    // Checks if provided password is correct one.
    static void testPassword(const QString& password, const QByteArray& data);

    // Decides whether file is highly probably encrypted or not.
    static bool isEncrypted(const QByteArray& data);

    // Returns output of "openssl version" or "not installed".
    static QString openSslVersion();

    // Returns list of supported cipher algorithms.
    static QStringList ciphers();

  private:
    static QString openSslBinaryPath();
};

#endif // CRYPTOFACTORY_H
