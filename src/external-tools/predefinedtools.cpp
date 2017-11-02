// For license of this file, see <project-root-folder>/LICENSE.md.

#include "external-tools/predefinedtools.h"

PredefinedTools::PredefinedTools() {}

QString PredefinedTools::toBase64(QString data) {
  return data.toUtf8().toBase64();
}

QString PredefinedTools::fromBase64(QString data) {
  return QByteArray::fromBase64(data.toUtf8());
}

QString PredefinedTools::toBase64Url(QString data) {
  return data.toUtf8().toBase64(QByteArray::Base64Option::Base64UrlEncoding);
}

QString PredefinedTools::fromBase64Url(QString data) {
  return QByteArray::fromBase64(data.toUtf8(), QByteArray::Base64Option::Base64UrlEncoding);
}
