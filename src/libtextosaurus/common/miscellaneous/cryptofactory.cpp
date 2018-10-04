// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "common/miscellaneous/cryptofactory.h"

#include "3rd-party/qtaes/qaesencryption.h"
#include "common/exceptions/ioexception.h"
#include "definitions/definitions.h"

#include <QCryptographicHash>
#include <QMessageAuthenticationCode>

bool CryptoFactory::isEncrypted(QFile& file) {
  bool was_open = file.isOpen();

  if (!was_open && !file.open(QIODevice::OpenModeFlag::ReadOnly)) {
    return false;
  }

  bool border_1_found = false;
  bool border_2_found = false;

  // Check bytes according to layout.
  if (file.seek(0)) {
    QByteArray byte_0 = file.read(1);

    border_1_found = !byte_0.isEmpty() && byte_0.at(0) == CRYPTO_FORMAT_BOUNDARY;
  }

  if (border_1_found && file.seek(65)) {
    QByteArray byte_65 = file.read(1);

    border_2_found = !byte_65.isEmpty() && byte_65.at(0) == CRYPTO_FORMAT_BOUNDARY;
  }

  if (!was_open) {
    file.close();
  }

  return border_1_found && border_2_found;
}

QByteArray CryptoFactory::encryptData(QString password, QByteArray data) {
  if (data.isEmpty()) {
    return QByteArray();
  }

  if (password.isEmpty()) {
    throw ApplicationException(QObject::tr("cannot encrypt file with empty password"));
  }

  auto utfpass = password.toUtf8();
  auto shahash = QCryptographicHash::hash(utfpass, QCryptographicHash::Algorithm::Sha3_512);
  auto iv = shahash.left(16);
  auto encpayload = QAESEncryption::Crypt(QAESEncryption::AES_256, QAESEncryption::Mode::CBC,
                                          data, shahash, iv, QAESEncryption::Padding::ISO);

  if (encpayload.isEmpty()) {
    throw ApplicationException(QObject::tr("some weird error appeared when encrypting the file"));
  }

  auto hmachash = QMessageAuthenticationCode::hash(encpayload, utfpass, QCryptographicHash::Algorithm::Sha3_512);

  return QByteArray(1, CRYPTO_FORMAT_BOUNDARY) + hmachash + QByteArray(1, CRYPTO_FORMAT_BOUNDARY) + encpayload;
}

QByteArray CryptoFactory::decryptData(QString password, QFile& file) {
  if (password.isEmpty()) {
    throw ApplicationException(QObject::tr("cannot decrypt file with empty password"));
  }

  bool was_open = file.isOpen();

  if (!was_open && !file.open(QIODevice::OpenModeFlag::ReadOnly)) {
    throw IOException(QObject::tr("insufficient permissions"));
  }

  if (file.size() <= 0) {
    if (!was_open) {
      file.close();
    }

    return QByteArray();
  }

  if (file.seek(1)) {
    QByteArray hmachash = file.read(64);

    if (file.seek(66)) {
      QByteArray encpayload = file.readAll();
      auto utfpass = password.toUtf8();
      auto hmachash_check = QMessageAuthenticationCode::hash(encpayload, utfpass, QCryptographicHash::Algorithm::Sha3_512);

      if (hmachash == hmachash_check) {
        // HMAC hash from file and checking HMAC are the same.
        // Decrypt payload.
        auto shahash = QCryptographicHash::hash(utfpass, QCryptographicHash::Algorithm::Sha3_512);
        auto iv = shahash.left(16);
        auto payload = QAESEncryption::Decrypt(QAESEncryption::AES_256, QAESEncryption::Mode::CBC,
                                               encpayload, shahash, iv, QAESEncryption::Padding::ISO);

        payload = QAESEncryption::RemovePadding(payload, QAESEncryption::Padding::ISO);

        if (!was_open) {
          file.close();
        }

        return payload;
      }
      else {
        if (!was_open) {
          file.close();
        }

        throw ApplicationException("incorrect password");
      }
    }
  }

  if (!was_open) {
    file.close();
  }

  throw ApplicationException("unspecified error when decrypting file");
}

bool CryptoFactory::isPasswordCorrect(QString password, QFile& file) {
  if (password.isEmpty()) {
    return false;
  }

  bool was_open = file.isOpen();

  if (!was_open && !file.open(QIODevice::OpenModeFlag::ReadOnly)) {
    throw IOException(QObject::tr("insufficient permissions"));
  }

  if (file.size() <= 0) {
    if (!was_open) {
      file.close();
    }

    return false;
  }

  bool correct_pw = false;

  if (file.seek(1)) {
    QByteArray hmachash = file.read(64);

    if (file.seek(66)) {
      QByteArray encpayload = file.readAll();
      auto utfpass = password.toUtf8();
      auto hmachash_check = QMessageAuthenticationCode::hash(encpayload, utfpass, QCryptographicHash::Algorithm::Sha3_512);

      correct_pw = hmachash == hmachash_check;
    }
  }

  if (!was_open) {
    file.close();
  }

  return correct_pw;
}
