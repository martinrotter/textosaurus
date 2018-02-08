// For license of this file, see <project-root-folder>/LICENSE.md.

#include "external-tools/predefinedtools.h"

#include "definitions/definitions.h"
#include "miscellaneous/application.h"
#include "network-web/networkfactory.h"

#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRegularExpression>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

QString PredefinedTools::sendToHastebin(const QString& data, bool* ok) {
  QByteArray output;
  QString content = QString("%1").arg(data);
  NetworkResult result = NetworkFactory::performNetworkOperation(PASTEBIN_HASTE_POST,
                                                                 DOWNLOAD_TIMEOUT,
                                                                 content.toUtf8(),
                                                                 output,
                                                                 QNetworkAccessManager::Operation::PostOperation);

  if (result.first == QNetworkReply::NetworkError::NoError) {
    *ok = true;

    QJsonDocument json_doc = QJsonDocument::fromJson(output);

    return PASTEBIN_HASTE + json_doc.object()["key"].toString();
  }
  else {
    *ok = false;
    return NetworkFactory::networkErrorText(result.first);
  }
}

QString PredefinedTools::sendToClbin(const QString& data, bool* ok) {
  QByteArray output;
  QString content = QString("clbin=%1").arg(data);
  NetworkResult result = NetworkFactory::performNetworkOperation(PASTEBIN_CLBIN,
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
  NetworkResult result = NetworkFactory::performNetworkOperation(PASTEBIN_IXIO,
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
  NetworkResult result = NetworkFactory::performNetworkOperation(PASTEBIN_SPRUNGE,
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

  NetworkResult result = NetworkFactory::performNetworkOperation(PASTEBIN_GITHUB,
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

    if (reader.error() != QXmlStreamReader::Error::NoError) {
      break;
    }

    if (!reader.isWhitespace() &&
        reader.tokenType() != QXmlStreamReader::TokenType::Invalid &&
        reader.tokenType() != QXmlStreamReader::TokenType::NoToken) {
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

QString PredefinedTools::xmlBeautifyFile(const QString& xml_file, bool* ok) {
  QFile file(xml_file);
  QFile file_out(xml_file + ".out654321");

  if (!file.open(QIODevice::OpenModeFlag::ReadOnly)) {
    *ok = false;
    return file.errorString();
  }

  if (!file_out.open(QIODevice::OpenModeFlag::WriteOnly | QIODevice::OpenModeFlag::WriteOnly)) {
    *ok = false;
    file.close();
    return file_out.errorString();
  }

  QXmlStreamReader reader(&file);
  QXmlStreamWriter writer(&file_out);

  writer.setAutoFormatting(true);
  writer.setAutoFormattingIndent(2);

  while (!reader.atEnd()) {
    reader.readNext();

    if (reader.error() != QXmlStreamReader::Error::NoError) {
      break;
    }

    if (!reader.isWhitespace() &&
        reader.tokenType() != QXmlStreamReader::TokenType::Invalid &&
        reader.tokenType() != QXmlStreamReader::TokenType::NoToken) {
      writer.writeCurrentToken(reader);
    }
  }

  file.close();
  file_out.close();

  if (reader.hasError()) {
    file_out.remove();

    *ok = false;
    return reader.errorString();
  }
  else {
    auto original_filename = file.fileName();

    file.remove();
    file_out.rename(original_filename);

    *ok = true;
    return QString();
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

    if (reader.error() != QXmlStreamReader::Error::NoError) {
      break;
    }

    if (!reader.isWhitespace() &&
        reader.tokenType() != QXmlStreamReader::TokenType::Invalid &&
        reader.tokenType() != QXmlStreamReader::TokenType::NoToken) {
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

QString PredefinedTools::currentDate(const QString& data, bool* ok) {
  Q_UNUSED(data)
  Q_UNUSED(ok)

  return qApp->localization()->loadedLocale().toString(QDateTime::currentDateTime(),
                                                       qApp->localization()->loadedLocale().dateFormat(QLocale::FormatType::ShortFormat));
}

QString PredefinedTools::currentTime(const QString& data, bool* ok) {
  Q_UNUSED(data)
  Q_UNUSED(ok)

  return qApp->localization()->loadedLocale().toString(QDateTime::currentDateTime(),
                                                       qApp->localization()->loadedLocale().timeFormat(QLocale::FormatType::ShortFormat));
}

QString PredefinedTools::formattedDateTime(const QString& data, bool* ok) {
  Q_UNUSED(ok)

  return qApp->localization()->loadedLocale().toString(QDateTime::currentDateTime(), data);
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

QString PredefinedTools::toTitleCase(const QString& data, bool* ok) {
  Q_UNUSED(ok)

  if (data.isEmpty()) {
    return data;
  }
  else {
    QString result = data;
    QRegularExpression regexp(QSL("(\\s|^)([^\\s\\d])"));
    QRegularExpressionMatchIterator match_iter = regexp.globalMatch(result);

    while (match_iter.hasNext()) {
      QRegularExpressionMatch next_match = match_iter.next();
      QString capt = next_match.captured(2);

      result[next_match.capturedStart(2)] = capt.at(0).toUpper();
    }

    return result;
  }
}

QString PredefinedTools::toSentenceCase(const QString& data, bool* ok) {
  Q_UNUSED(ok)

  if (data.isEmpty()) {
    return data;
  }
  else {
    return data.at(0).toUpper() + data.mid(1);
  }
}

QString PredefinedTools::invertCase(const QString& data, bool* ok) {
  Q_UNUSED(ok)

  if (data.isEmpty()) {
    return data;
  }
  else {
    QString result;

    for (int i = 0; i < data.size(); i++) {
      QChar chr = data[i];

      result += chr.isUpper() ? chr.toLower() : chr.toUpper();
    }

    return result;
  }
}

QString PredefinedTools::toHtmlEscaped(const QString& data, bool* ok) {
  Q_UNUSED(ok)
  return data.toHtmlEscaped();
}
