// For license of this file, see <project-root-folder>/LICENSE.md.

#include "miscellaneous/textfactory.h"

#include "definitions/definitions.h"
#include "exceptions/applicationexception.h"
#include "miscellaneous/application.h"
#include "miscellaneous/iofactory.h"
#include "miscellaneous/simplecrypt/simplecrypt.h"
#include "miscellaneous/textapplication.h"

#include <QDir>
#include <QLocale>
#include <QMenu>
#include <QString>
#include <QStringList>
#include <QTextCodec>

quint64 TextFactory::s_encryptionKey = 0x0;

TextFactory::TextFactory() {}

bool TextFactory::isCaseInsensitiveLessThan(const QString& lhs, const QString& rhs) {
  return lhs.toLower() < rhs.toLower();
}

int TextFactory::stringHeight(const QString& string, const QFontMetrics& metrics) {
  const int count_lines = string.split(QL1C('\n')).size();

  return metrics.height() * count_lines;
}

int TextFactory::stringWidth(const QString& string, const QFontMetrics& metrics) {
  const QStringList lines = string.split(QL1C('\n'));
  int width = 0;

  foreach (const QString& line, lines) {
    int line_width = metrics.width(line);

    if (line_width > width) {
      width = line_width;
    }
  }

  return width;
}

QDateTime TextFactory::parseDateTime(const QString& date_time) {
  const QString input_date = date_time.simplified();
  QDateTime dt;
  QTime time_zone_offset;
  const QLocale locale(QLocale::C);
  bool positive_time_zone_offset = false;
  QStringList date_patterns;

  date_patterns << QSL("yyyy-MM-ddTHH:mm:ss") << QSL("MMM dd yyyy hh:mm:ss") <<
    QSL("MMM d yyyy hh:mm:ss") << QSL("ddd, dd MMM yyyy HH:mm:ss") <<
    QSL("dd MMM yyyy") << QSL("yyyy-MM-dd HH:mm:ss.z") << QSL("yyyy-MM-dd") <<
    QSL("yyyy") << QSL("yyyy-MM") << QSL("yyyy-MM-dd") << QSL("yyyy-MM-ddThh:mm") <<
    QSL("yyyy-MM-ddThh:mm:ss");
  QStringList timezone_offset_patterns;

  timezone_offset_patterns << QSL("+hh:mm") << QSL("-hh:mm") << QSL("+hhmm")
                           << QSL("-hhmm") << QSL("+hh") << QSL("-hh");

  if (input_date.size() >= 6) {
    foreach (const QString& pattern, timezone_offset_patterns) {
      time_zone_offset = QTime::fromString(input_date.right(pattern.size()), pattern);

      if (time_zone_offset.isValid()) {
        positive_time_zone_offset = pattern.at(0) == QL1C('+');
        break;
      }
    }
  }

  // Iterate over patterns and check if input date/time matches the pattern.
  foreach (const QString& pattern, date_patterns) {
    dt = locale.toDateTime(input_date.left(pattern.size()), pattern);

    if (dt.isValid()) {
      // Make sure that this date/time is considered UTC.
      dt.setTimeSpec(Qt::UTC);

      if (time_zone_offset.isValid()) {
        // Time zone offset was detected.
        if (positive_time_zone_offset) {
          // Offset is positive, so we have to subtract it to get
          // the original UTC.
          return dt.addSecs(-QTime(0, 0, 0, 0).secsTo(time_zone_offset));
        }
        else {
          // Vice versa.
          return dt.addSecs(QTime(0, 0, 0, 0).secsTo(time_zone_offset));
        }
      }
      else {
        return dt;
      }
    }
  }

  // Parsing failed, return invalid datetime.
  return QDateTime();
}

void TextFactory::initializeEncodingMenu(QMenu* const menu) {
  QList<int> mibs = QTextCodec::availableMibs();
  QStringList codecs; codecs.reserve(mibs.size());

  QMap<QString, QMenu*> submenus;

  submenus["Big"] = new QMenu(QSL("Big"), menu);
  submenus["IBM"] = new QMenu(QSL("IBM"), menu);
  submenus["ISO"] = new QMenu(QSL("ISO"), menu);
  submenus["UTF"] = new QMenu(QSL("UTF"), menu);
  submenus["windows"] = new QMenu(QSL("windows"), menu);

  for (int mib : mibs) {
    QString name = QTextCodec::codecForMib(mib)->name();

    if (!codecs.contains(name)) {
      codecs.append(name);
    }
  }

  qSort(codecs);
  QString predefined_group = QString("^(") + submenus.keys().join(QL1C('|')) + QString(")\\S+");
  QRegularExpression predefined_group_regex(predefined_group);

  predefined_group_regex.setPatternOptions(QRegularExpression::PatternOption::CaseInsensitiveOption);

  menu->addMenu(submenus["Big"]);
  menu->addMenu(submenus["IBM"]);
  menu->addMenu(submenus["ISO"]);
  menu->addMenu(submenus["UTF"]);
  menu->addMenu(submenus["windows"]);

  foreach (const QString& codec, codecs) {
    auto match = predefined_group_regex.match(codec);
    QAction* act;

    if (match.hasMatch()) {
      QString capture = match.captured(1);

      if (submenus.contains(capture)) {
        act = submenus[capture]->addAction(QString("&") + codec);
      }
    }
    else {
      act = menu->addAction(QString("&") + codec);
    }

    act->setData(codec);
  }
}

QDateTime TextFactory::parseDateTime(qint64 milis_from_epoch) {
  return QDateTime::fromMSecsSinceEpoch(milis_from_epoch);
}

QString TextFactory::encrypt(const QString& text) {
  return SimpleCrypt(initializeSecretEncryptionKey()).encryptToString(text);
}

QString TextFactory::decrypt(const QString& text) {
  return SimpleCrypt(initializeSecretEncryptionKey()).decryptToString(text);
}

QString TextFactory::shorten(const QString& input, int text_length_limit) {
  if (input.size() > text_length_limit) {
    return input.left(text_length_limit - ELLIPSIS_LENGTH) + QString(ELLIPSIS_LENGTH, QL1C('.'));
  }
  else {
    return input;
  }
}

quint64 TextFactory::initializeSecretEncryptionKey() {
  if (s_encryptionKey == 0x0) {
    // Check if file with encryption key exists.
    QString encryption_file_path = qApp->settings()->pathName() + QDir::separator() + ENCRYPTION_FILE_NAME;

    try {
      s_encryptionKey = (quint64) QString(IOFactory::readFile(encryption_file_path)).toLongLong();
    }
    catch (ApplicationException) {
      // Well, key does not exist or is invalid, generate and save one.
      s_encryptionKey = generateSecretEncryptionKey();
      IOFactory::writeFile(encryption_file_path, QString::number(s_encryptionKey).toLocal8Bit());
    }
  }

  return s_encryptionKey;
}

quint64 TextFactory::generateSecretEncryptionKey() {
  return RAND_MAX * qrand() + qrand();
}
