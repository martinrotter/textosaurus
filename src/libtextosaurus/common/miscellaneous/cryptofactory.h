// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifndef CRYPTOFACTORY_H
#define CRYPTOFACTORY_H

#include <QFile>

// Simple encryption for byte-arrays or files based on AES-CBC.
//
// Layout of "encrypted file:
//  | 1 byte  | 64 bytes           | 1 byte  | X bytes                |
//  |---------|--------------------|---------|------------------------|
//  | dec(30) | HMAC SHA3-512 hash | dec(30) | AES-CBC-encrypted data |
//
// Encryption process:
//  1. User enter passphrase "pass".
//  2. "pass" is converted to UTF-8 byte array "utfpass".
//  3. Payload data byte array is encrypted with AES-CBC -> "encpayload":
//    a. SHA3-512 hash "shahash" is made from "utfpass" - this hash is used as key.
//    b. First 16 bytes of "shahash" are used as initialization vector.
//  4. "utfpass" and array of encrypted data "encpayload" are used to calculate HMAC hash "hmachash".
//  5. All parts are assembled into proper layout.
//
// Decryption process:
//  1. Extract HMAC hash "hmachash".
//  2. Extract encrypted payload data "encpayload".
//  3. User enters passphrase which is converted to UTF-8 byte array "utfpass".
//  4. "utfpass" and array of encrypted data "encpayload" are used to calculate second HMAC hash "hmachash-check".
//    a. If "hmachash" and "hmachash-check" are equal, then user entered correct encryption passphrase, proceed to 5.
//    b. If those two HMACs are not equal, then user entered invalid passphrase, go to 3 or abort.
//  5. Decrypt "encpayload" with AES-CBC -> "payload":
//    a. SHA3-512 hash "shahash" is made from "utfpass" - this hash is used as key.
//    b. First 16 bytes of "shahash" are used as initialization vector.
//
// Checking if file is encrypted:
//  1. Check if file[0] and file[65] equal do dec(30).
//  2. If they do equal, file is probably "encrypted", otherwise it is surely not encrypted.
class CryptoFactory {
  public:
    CryptoFactory() = delete;
    ~CryptoFactory() = delete;

    // Encrypts data with given password.
    static QByteArray encryptData(const QString& password, const QByteArray& data);

    // Decrypts data with given password.
    // Throws exception if password is invalid or other
    // problem appears.
    static QByteArray decryptData(const QString& password, QFile& file);

    // Checks if provided password is correct one.
    static bool isPasswordCorrect(const QString& password, QFile& file);

    // Decides whether file is highly probably encrypted or not.
    static bool isEncrypted(QFile& file);
};

#endif // CRYPTOFACTORY_H
