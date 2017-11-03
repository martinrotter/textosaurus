// For license of this file, see <project-root-folder>/LICENSE.md.

#include "external-tools/predefinedtools.h"

#include "definitions/definitions.h"
#include "network-web/networkfactory.h"

#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRegularExpression>

PredefinedTools::PredefinedTools() {}

QString PredefinedTools::sendToClbin(const QString& data, bool* ok) {
  QByteArray output;
  QString content = QString("clbin=%1").arg(data);
  NetworkResult result = NetworkFactory::performNetworkOperation(QSL(PASTEBIN_CLBIN),
                                                                 DOWNLOAD_TIMEOUT,
                                                                 content.toUtf8(),
                                                                 output,
                                                                 QNetworkAccessManager::Operation::PostOperation);

  if (result.first == QNetworkReply::NetworkError::NoError) {
    *ok = true;
    return QString(output).remove(QRegularExpression(QSL("\\s")));
  }
  else {
    *ok = false;
    return NetworkFactory::networkErrorText(result.first);
  }
}

QString PredefinedTools::sendToIxio(const QString& data, bool* ok) {
  QByteArray output;
  QString content = QString("f:1=%1").arg(data);
  NetworkResult result = NetworkFactory::performNetworkOperation(QSL(PASTEBIN_IXIO),
                                                                 DOWNLOAD_TIMEOUT,
                                                                 content.toUtf8(),
                                                                 output,
                                                                 QNetworkAccessManager::Operation::PostOperation);

  if (result.first == QNetworkReply::NetworkError::NoError) {
    *ok = true;
    return QString(output).remove(QRegularExpression(QSL("\\s")));
  }
  else {
    *ok = false;
    return NetworkFactory::networkErrorText(result.first);
  }
}

QString PredefinedTools::sendToSprunge(const QString& data, bool* ok) {
  QByteArray output;
  QString content = QString("sprunge=%1").arg(data);
  NetworkResult result = NetworkFactory::performNetworkOperation(QSL(PASTEBIN_SPRUNGE),
                                                                 DOWNLOAD_TIMEOUT,
                                                                 content.toUtf8(),
                                                                 output,
                                                                 QNetworkAccessManager::Operation::PostOperation);

  if (result.first == QNetworkReply::NetworkError::NoError) {
    *ok = true;
    return QString(output).remove(QRegularExpression(QSL("\\s")));
  }
  else {
    *ok = false;
    return NetworkFactory::networkErrorText(result.first);
  }
}

QString PredefinedTools::sendToGithub(const QString& data, bool* ok) {
  QByteArray output;
  QJsonDocument content_doc;
  QJsonObject files_obj;

  files_obj["file.txt"] = QJsonObject {
    {QSL("content"), data}
  };
  content_doc.setObject(QJsonObject {
    {"public", true},
    {"files", QJsonObject {
       {"file.cpp", QJsonObject {
          {"content", data}
        }}
     }}
  });

  NetworkResult result = NetworkFactory::performNetworkOperation(QSL(PASTEBIN_GITHUB),
                                                                 DOWNLOAD_TIMEOUT,
                                                                 content_doc.toJson(QJsonDocument::JsonFormat::Compact),
                                                                 output,
                                                                 QNetworkAccessManager::Operation::PostOperation);

  if (result.first == QNetworkReply::NetworkError::NoError) {
    *ok = true;
    return QJsonDocument::fromJson(output).object()["html_url"].toString().remove(QRegularExpression(QSL("\\s")));
  }
  else {
    *ok = false;
    return NetworkFactory::networkErrorText(result.first);
  }
}

QString PredefinedTools::currentDateTime(const QString& data, bool* ok) {
  Q_UNUSED(data)
  Q_UNUSED(ok)

  return QDateTime::currentDateTime().toString(Qt::DateFormat::ISODate);
}

QString PredefinedTools::toBase64(const QString& data, bool* ok) {
  Q_UNUSED(ok)

  return data.toUtf8().toBase64();
}

QString PredefinedTools::fromBase64(const QString& data, bool* ok) {
  Q_UNUSED(ok)

  return QByteArray::fromBase64(data.toUtf8());
}

QString PredefinedTools::toBase64Url(const QString& data, bool* ok) {
  Q_UNUSED(ok)

  return data.toUtf8().toBase64(QByteArray::Base64Option::Base64UrlEncoding);
}

QString PredefinedTools::fromBase64Url(const QString& data, bool* ok) {
  Q_UNUSED(ok)

  return QByteArray::fromBase64(data.toUtf8(), QByteArray::Base64Option::Base64UrlEncoding);
}
