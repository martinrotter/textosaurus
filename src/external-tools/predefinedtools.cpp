// For license of this file, see <project-root-folder>/LICENSE.md.

#include "external-tools/predefinedtools.h"

#include "definitions/definitions.h"
#include "network-web/networkfactory.h"

#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRegularExpression>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

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

QString PredefinedTools::jsonBeautify(const QString& data, bool* ok) {
  QJsonDocument doc = QJsonDocument::fromJson(data.toUtf8());

  if (doc.isNull()) {
    *ok = false;
    return QObject::tr("Parsing of JSON document failed.");
  }
  else {
    *ok = true;
    return doc.toJson(QJsonDocument::JsonFormat::Indented);
  }
}

QString PredefinedTools::jsonMinify(const QString& data, bool* ok) {
  QJsonDocument doc = QJsonDocument::fromJson(data.toUtf8());

  if (doc.isNull()) {
    *ok = false;
    return QObject::tr("Parsing of JSON document failed.");
  }
  else {
    *ok = true;
    return doc.toJson(QJsonDocument::JsonFormat::Compact);
  }
}

QString PredefinedTools::xmlCheck(const QString& data, bool* ok) {
  QXmlStreamReader reader(data.toUtf8());

  while (!reader.atEnd()) {
    if (reader.hasError()) {
      break;
    }
    else {
      reader.readNext();
    }
  }

  *ok = !reader.hasError();
  return *ok ? QObject::tr("XML is well-formed.") : reader.errorString();
}

QString PredefinedTools::xmlBeautify(const QString& data, bool* ok) {
  QByteArray input = data.toUtf8();
  QString xml_out;
  QXmlStreamReader reader(input);
  QXmlStreamWriter writer(&xml_out);

  writer.setAutoFormatting(true);
  writer.setAutoFormattingIndent(2);

  while (!reader.atEnd()) {
    reader.readNext();
    if (!reader.isWhitespace()) {
      writer.writeCurrentToken(reader);
    }
  }

  if (reader.hasError()) {
    *ok = false;
    return reader.errorString();
  }
  else {
    *ok = true;
    return xml_out;
  }
}

QString PredefinedTools::xmlLinearize(const QString& data, bool* ok) {
  QByteArray input = data.toUtf8();
  QString xml_out;
  QXmlStreamReader reader(input);
  QXmlStreamWriter writer(&xml_out);

  writer.setAutoFormatting(false);

  while (!reader.atEnd()) {
    reader.readNext();
    if (!reader.isWhitespace()) {
      writer.writeCurrentToken(reader);
    }
  }

  if (reader.hasError()) {
    *ok = false;
    return reader.errorString();
  }
  else {
    *ok = true;
    return xml_out;
  }
}

QString PredefinedTools::currentDateTime(const QString& data, bool* ok) {
  Q_UNUSED(data)
  Q_UNUSED(ok)

  return QDateTime::currentDateTime().toString(Qt::DateFormat::ISODate);
}

QString PredefinedTools::toUrlEncoded(const QString& data, bool* ok) {
  Q_UNUSED(ok)
  return QUrl::toPercentEncoding(data);
}

QString PredefinedTools::fromUrlEncoded(const QString& data, bool* ok) {
  Q_UNUSED(ok)
  return QUrl::fromPercentEncoding(data.toUtf8());
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

QString PredefinedTools::toLower(const QString& data, bool* ok) {
  Q_UNUSED(ok)
  return data.toLower();
}

QString PredefinedTools::toUpper(const QString& data, bool* ok) {
  Q_UNUSED(ok)
  return data.toUpper();
}

QString PredefinedTools::toHtmlEscaped(const QString& data, bool* ok) {
  Q_UNUSED(ok)
  return data.toHtmlEscaped();
}
