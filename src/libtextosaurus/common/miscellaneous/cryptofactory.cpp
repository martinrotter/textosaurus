// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "common/miscellaneous/cryptofactory.h"

bool CryptoFactory::isEncrypted(QString file_path) {
  return false;
}

bool CryptoFactory::verifyPassword(QString password, QString file_path) {
  return false;
}

QByteArray CryptoFactory::encryptData(QString password, QByteArray data) {
  return "";
}

QByteArray CryptoFactory::decryptData(QString password) {
  return "";
}
