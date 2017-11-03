// For license of this file, see <project-root-folder>/LICENSE.md.

#include "external-tools/predefinedtools.h"

#include "definitions/definitions.h"
#include "network-web/networkfactory.h"

#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRegularExpression>

PredefinedTools::PredefinedTools() {}

QString PredefinedTools::sendToClbin(const QString& data) {
  QByteArray output;
  QString content = QString("clbin=%1").arg(data);
  NetworkResult result = NetworkFactory::performNetworkOperation(QSL(PASTEBIN_CLBIN),
                                                                 DOWNLOAD_TIMEOUT,
                                                                 content.toUtf8(),
                                                                 output,
                                                                 QNetworkAccessManager::Operation::PostOperation);

  if (result.first == QNetworkReply::NetworkError::NoError) {
    return QString(output).remove(QRegularExpression(QSL("\\s")));
  }
  else {
    return QString();
  }
}

QString PredefinedTools::sendToIxio(const QString& data) {
  QByteArray output;
  QString content = QString("f:1=%1").arg(data);
  NetworkResult result = NetworkFactory::performNetworkOperation(QSL(PASTEBIN_IXIO),
                                                                 DOWNLOAD_TIMEOUT,
                                                                 content.toUtf8(),
                                                                 output,
                                                                 QNetworkAccessManager::Operation::PostOperation);

  if (result.first == QNetworkReply::NetworkError::NoError) {
    return QString(output).remove(QRegularExpression(QSL("\\s")));
  }
  else {
    return QString();
  }
}

QString PredefinedTools::sendToSprunge(const QString& data) {
  QByteArray output;
  QString content = QString("sprunge=%1").arg(data);
  NetworkResult result = NetworkFactory::performNetworkOperation(QSL(PASTEBIN_SPRUNGE),
                                                                 DOWNLOAD_TIMEOUT,
                                                                 content.toUtf8(),
                                                                 output,
                                                                 QNetworkAccessManager::Operation::PostOperation);

  if (result.first == QNetworkReply::NetworkError::NoError) {
    return QString(output).remove(QRegularExpression(QSL("\\s")));
  }
  else {
    return QString();
  }
}

QString PredefinedTools::sendToGithub(const QString& data) {
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

  auto aa = content_doc.toJson(QJsonDocument::JsonFormat::Compact);
  NetworkResult result = NetworkFactory::performNetworkOperation(QSL(PASTEBIN_GITHUB),
                                                                 DOWNLOAD_TIMEOUT,
                                                                 content_doc.toJson(QJsonDocument::JsonFormat::Compact),
                                                                 output,
                                                                 QNetworkAccessManager::Operation::PostOperation);

  if (result.first == QNetworkReply::NetworkError::NoError) {
    return QJsonDocument::fromJson(output).object()["html_url"].toString().remove(QRegularExpression(QSL("\\s")));
  }
  else {
    return QString();
  }
}

QString PredefinedTools::currentDateTime(const QString& data) {
  Q_UNUSED(data)
  return QDateTime::currentDateTime().toString(Qt::DateFormat::ISODate);
}

QString PredefinedTools::toBase64(const QString& data) {
  return data.toUtf8().toBase64();
}

QString PredefinedTools::fromBase64(const QString& data) {
  return QByteArray::fromBase64(data.toUtf8());
}

QString PredefinedTools::toBase64Url(const QString& data) {
  return data.toUtf8().toBase64(QByteArray::Base64Option::Base64UrlEncoding);
}

QString PredefinedTools::fromBase64Url(const QString& data) {
  return QByteArray::fromBase64(data.toUtf8(), QByteArray::Base64Option::Base64UrlEncoding);
}
