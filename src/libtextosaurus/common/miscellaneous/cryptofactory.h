// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifndef CRYPTOFACTORY_H
#define CRYPTOFACTORY_H

#include <QString>

// Simple encryption for byte-arrays or files
// based on AES
class CryptoFactory {
  public:
    CryptoFactory() = delete;
    ~CryptoFactory() = delete;

    // Takes input password and path to file and verifies
    // if given password is correct.
    static bool isEncrypted(QString file_path);
    static bool verifyPassword(QString password, QString file_path);
    static QByteArray encryptData(QString password, QByteArray data);
    static QByteArray decryptData(QString password);
};

#endif // CRYPTOFACTORY_H
