// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "saurus/external-tools/predefinedtools.h"

#include "common/network-web/networkfactory.h"
#include "definitions/definitions.h"
#include "saurus/miscellaneous/application.h"
#include "saurus/miscellaneous/textapplication.h"
#include "saurus/miscellaneous/textapplicationsettings.h"

#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRegularExpression>
#include <QTemporaryFile>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

QString PredefinedTools::reverse(const QString& data, bool& ok) {
  Q_UNUSED(ok)

  QString rev;

  for (auto chr = data.crbegin(); chr < data.crend(); chr++) {
    rev.append(*chr);
  }

  return rev;
}

QString PredefinedTools::sendToHastebin(const QString& data, bool& ok) {
  QByteArray output;
  QString content = QString("%1").arg(data);
  NetworkResult result = NetworkFactory::performNetworkOperation(PASTEBIN_HASTE_POST,
                                                                 DOWNLOAD_TIMEOUT,
                                                                 content.toUtf8(),
                                                                 output,
                                                                 QNetworkAccessManager::Operation::PostOperation);

  if (result.first == QNetworkReply::NetworkError::NoError) {
    ok = true;

    QJsonDocument json_doc = QJsonDocument::fromJson(output);

    return PASTEBIN_HASTE + json_doc.object()["key"].toString();
  }
  else {
    ok = false;
    return NetworkFactory::networkErrorText(result.first);
  }
}

QString PredefinedTools::sendToClbin(const QString& data, bool& ok) {
  QByteArray output;
  QString content = QString("clbin=%1").arg(data);
  NetworkResult result = NetworkFactory::performNetworkOperation(PASTEBIN_CLBIN,
                                                                 DOWNLOAD_TIMEOUT,
                                                                 content.toUtf8(),
                                                                 output,
                                                                 QNetworkAccessManager::Operation::PostOperation);

  if (result.first == QNetworkReply::NetworkError::NoError) {
    ok = true;
    return QString(output).remove(QRegularExpression(QSL("\\s")));
  }
  else {
    ok = false;
    return NetworkFactory::networkErrorText(result.first);
  }
}

QString PredefinedTools::sendToIxio(const QString& data, bool& ok) {
  QByteArray output;
  QString content = QString("f:1=%1").arg(data);
  NetworkResult result = NetworkFactory::performNetworkOperation(PASTEBIN_IXIO,
                                                                 DOWNLOAD_TIMEOUT,
                                                                 content.toUtf8(),
                                                                 output,
                                                                 QNetworkAccessManager::Operation::PostOperation);

  if (result.first == QNetworkReply::NetworkError::NoError) {
    ok = true;
    return QString(output).remove(QRegularExpression(QSL("\\s")));
  }
  else {
    ok = false;
    return NetworkFactory::networkErrorText(result.first);
  }
}

QString PredefinedTools::jsonBeautify(const QString& data, bool& ok) {
  QJsonDocument doc = QJsonDocument::fromJson(data.toUtf8());

  if (doc.isNull()) {
    ok = false;
    return QObject::tr("Parsing of JSON document failed.");
  }
  else {
    ok = true;
    return doc.toJson(QJsonDocument::JsonFormat::Indented);
  }
}

QString PredefinedTools::jsonMinify(const QString& data, bool& ok) {
  QJsonDocument doc = QJsonDocument::fromJson(data.toUtf8());

  if (doc.isNull()) {
    ok = false;
    return QObject::tr("Parsing of JSON document failed.");
  }
  else {
    ok = true;
    return doc.toJson(QJsonDocument::JsonFormat::Compact);
  }
}

QString PredefinedTools::xmlCheck(const QString& data, bool& ok) {
  QXmlStreamReader reader(data.toUtf8());

  while (!reader.atEnd()) {
    if (reader.hasError()) {
      break;
    }

    reader.readNext();
  }

  ok = !reader.hasError();
  return ok ? QObject::tr("XML is well-formed.") : reader.errorString();
}

QString PredefinedTools::xmlBeautify(const QString& data, bool& ok) {
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
        reader.tokenType() != QXmlStreamReader::TokenType::NoToken &&
        reader.tokenType() != QXmlStreamReader::TokenType::StartDocument) {
      writer.writeCurrentToken(reader);
    }
  }

  if (reader.hasError()) {
    ok = false;
    return reader.errorString();
  }
  else {
    ok = true;
    return xml_out;
  }
}

QString PredefinedTools::xmlBeautifyFile(const QString& xml_file, bool& ok) {
  QFile file(xml_file);
  QTemporaryFile file_out;

  if (!file.open(QIODevice::OpenModeFlag::ReadWrite)) {
    ok = false;
    return file.errorString();
  }

  if (!file_out.open()) {
    ok = false;
    file.close();
    return file_out.errorString();
  }

  QXmlStreamReader reader(&file);
  QXmlStreamWriter writer(&file_out);
  QString xml_encoding;

  writer.setAutoFormatting(true);
  writer.setAutoFormattingIndent(2);

  while (!reader.atEnd()) {
    reader.readNext();

    if (reader.error() != QXmlStreamReader::Error::NoError) {
      break;
    }

    if (reader.tokenType() == QXmlStreamReader::TokenType::StartDocument) {
      xml_encoding = reader.documentEncoding().toString();

      if (xml_encoding.isEmpty()) {
        qWarning().noquote() << QSL("No XML encoding detected when beautifying XML file.");
      }
      else {
        writer.setCodec(xml_encoding.toUtf8().data());
      }
    }

    if (!reader.isWhitespace() &&
        reader.tokenType() != QXmlStreamReader::TokenType::Invalid &&
        reader.tokenType() != QXmlStreamReader::TokenType::NoToken) {
      writer.writeCurrentToken(reader);
    }
  }

  if (reader.hasError()) {
    file.close();
    file_out.close();
    ok = false;
    return reader.errorString();
  }
  else {
    file.seek(0);
    file_out.seek(0);

    while (!file_out.atEnd()) {
      file.write(file_out.readLine());
    }

    file.resize(file.pos());

    file_out.close();
    file.flush();
    file.close();

    ok = true;
    return QString();
  }
}

QString PredefinedTools::xmlLinearize(const QString& data, bool& ok) {
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
    ok = false;
    return reader.errorString();
  }
  else {
    ok = true;
    return xml_out;
  }
}

QString PredefinedTools::currentDateTime(const QString& data, bool& ok) {
  Q_UNUSED(data)
  Q_UNUSED(ok)

  return QLocale::system().toString(QDateTime::currentDateTime(),
                                    QLocale::system().dateTimeFormat(QLocale::FormatType::ShortFormat));
}

QString PredefinedTools::currentDate(const QString& data, bool& ok) {
  Q_UNUSED(data)
  Q_UNUSED(ok)

  return QLocale::system().toString(QDateTime::currentDateTime(),
                                    QLocale::system().dateFormat(QLocale::FormatType::ShortFormat));
}

QString PredefinedTools::currentTime(const QString& data, bool& ok) {
  Q_UNUSED(data)
  Q_UNUSED(ok)

  return QLocale::system().toString(QDateTime::currentDateTime(),
                                    QLocale::system().timeFormat(QLocale::FormatType::ShortFormat));
}

QString PredefinedTools::formattedDateTime(const QString& data, bool& ok) {
  Q_UNUSED(data)
  Q_UNUSED(ok)

  return QLocale::system().toString(QDateTime::currentDateTime(), qApp->textApplication()->settings()->dateTimeTimestampFormat());
}

QString PredefinedTools::toUrlEncoded(const QString& data, bool& ok) {
  Q_UNUSED(ok)
  return QUrl::toPercentEncoding(data);
}

QString PredefinedTools::fromUrlEncoded(const QString& data, bool& ok) {
  Q_UNUSED(ok)
  return QUrl::fromPercentEncoding(data.toUtf8());
}

QString PredefinedTools::toBase64(const QString& data, bool& ok) {
  Q_UNUSED(ok)

  return data.toUtf8().toBase64();
}

QString PredefinedTools::fromBase64(const QString& data, bool& ok) {
  Q_UNUSED(ok)

  return QByteArray::fromBase64(data.toUtf8());
}

QString PredefinedTools::toBase64Url(const QString& data, bool& ok) {
  Q_UNUSED(ok)

  return data.toUtf8().toBase64(QByteArray::Base64Option::Base64UrlEncoding);
}

QString PredefinedTools::fromBase64Url(const QString& data, bool& ok) {
  Q_UNUSED(ok)

  return QByteArray::fromBase64(data.toUtf8(), QByteArray::Base64Option::Base64UrlEncoding);
}

QString PredefinedTools::toLower(const QString& data, bool& ok) {
  Q_UNUSED(ok)
  return data.toLower();
}

QString PredefinedTools::toUpper(const QString& data, bool& ok) {
  Q_UNUSED(ok)
  return data.toUpper();
}

QString PredefinedTools::toTitleCase(const QString& data, bool& ok) {
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

QString PredefinedTools::toSentenceCase(const QString& data, bool& ok) {
  Q_UNUSED(ok)

  if (data.isEmpty()) {
    return data;
  }
  else {
    return data.at(0).toUpper() + data.mid(1);
  }
}

QString PredefinedTools::invertCase(const QString& data, bool& ok) {
  Q_UNUSED(ok)

  if (data.isEmpty()) {
    return data;
  }
  else {
    QString result;

    for (QChar chr : data) {
      result += chr.isUpper() ? chr.toLower() : chr.toUpper();
    }

    return result;
  }
}

QString PredefinedTools::toHtmlEscaped(const QString& data, bool& ok) {
  Q_UNUSED(ok)
  return data.toHtmlEscaped();
}
