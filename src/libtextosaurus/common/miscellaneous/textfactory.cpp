// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "common/miscellaneous/textfactory.h"

#include "common/exceptions/applicationexception.h"
#include "common/miscellaneous/iofactory.h"
#include "definitions/definitions.h"

// *INDENT-OFF*
#include PATH(SCINTILLA_DIR,include/Scintilla.h)

// *INDENT-ON*

#include "3rd-party/uchardet/uchardet.h"

#include <QDir>
#include <QLocale>
#include <QMenu>
#include <QRegularExpression>
#include <QString>
#include <QStringList>
#include <QTextCodec>

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

void TextFactory::initializeEncodingMenu(QMenu* const menu, bool checkable) {
  QList<int> mibs = QTextCodec::availableMibs();
  QStringList codecs; codecs.reserve(mibs.size());

  QMap<QString, QMenu*> submenus;

  submenus["Big"] = new QMenu(QSL("&Big"), menu);
  submenus["IBM"] = new QMenu(QSL("&IBM"), menu);
  submenus["ISO"] = new QMenu(QSL("&ISO"), menu);
  submenus["KOI8"] = new QMenu(QSL("&KOI8"), menu);
  submenus["UTF"] = new QMenu(QSL("&UTF"), menu);
  submenus["windows"] = new QMenu(QSL("&windows"), menu);

  for (int mib : mibs) {
    QString name = QTextCodec::codecForMib(mib)->name();

    if (!codecs.contains(name)) {
      codecs.append(name);
    }
  }

  std::sort(codecs.begin(), codecs.end(), [](const QString& arg1, const QString& arg2) {
    return arg1.compare(arg2, Qt::CaseSensitivity::CaseInsensitive) < 0;
  });

  QString predefined_group = QString("^(") + submenus.keys().join(QL1C('|')) + QString(")\\S+");
  QRegularExpression predefined_group_regex(predefined_group);

  predefined_group_regex.setPatternOptions(QRegularExpression::PatternOption::CaseInsensitiveOption);

  menu->addMenu(submenus["Big"]);
  menu->addMenu(submenus["IBM"]);
  menu->addMenu(submenus["ISO"]);
  menu->addMenu(submenus["KOI8"]);
  menu->addMenu(submenus["UTF"]);
  menu->addMenu(submenus["windows"]);

  QActionGroup* grp_actions = checkable ? new QActionGroup(menu) : nullptr;

  submenus["Big"]->menuAction()->setActionGroup(grp_actions);
  submenus["IBM"]->menuAction()->setActionGroup(grp_actions);
  submenus["ISO"]->menuAction()->setActionGroup(grp_actions);
  submenus["KOI8"]->menuAction()->setActionGroup(grp_actions);
  submenus["UTF"]->menuAction()->setActionGroup(grp_actions);
  submenus["windows"]->menuAction()->setActionGroup(grp_actions);

  foreach (const QString& codec, codecs) {
    auto match = predefined_group_regex.match(codec);
    QAction* act = nullptr;

    if (match.hasMatch()) {
      QString capture = match.captured(1);

      if (submenus.contains(capture)) {
        act = submenus[capture]->addAction(QString("&") + codec);
      }
    }
    else {
      act = menu->addAction(QString("&") + codec);
    }

    if (act != nullptr) {
      act->setData(codec);

      if (checkable) {
        act->setCheckable(true);
        grp_actions->addAction(act);
      }
    }
  }
}

QByteArray TextFactory::detectEncoding(const QByteArray& file_head_chunk) {
  // We read first chunk of file and try to detect encoding.
  int len = file_head_chunk.length();
  const char* buf = file_head_chunk.data();
  uchardet_t ud = uchardet_new();

  uchardet_handle_data(ud, buf, size_t(len));
  uchardet_data_end(ud);
  const char* cs = uchardet_get_charset(ud);
  QByteArray encoding_name = QByteArray(cs);

  uchardet_delete(ud);

  if (encoding_name == QString("ASCII")) {
    return DEFAULT_TEXT_FILE_ENCODING;
  }
  else {
    return encoding_name;
  }
}

int TextFactory::detectEol(const QByteArray& file_head_chunk) {
  if (file_head_chunk.contains(QString("\r\n").toLocal8Bit().constData())) {
    return SC_EOL_CRLF;
  }
  else if (file_head_chunk.contains(QString("\n").toLocal8Bit().constData())) {
    return SC_EOL_LF;
  }
  else if (file_head_chunk.contains(QString("\r").toLocal8Bit().constData())) {
    return SC_EOL_CR;
  }
  else {
    return -1;
  }
}

QString TextFactory::eolDescriptionFromCode(int eol_code) {
  switch (eol_code) {
    case SC_EOL_CRLF:
      return QSL("CRLF");

    case SC_EOL_CR:
      return QSL("CR");

    case SC_EOL_LF:
      return QSL("LF");

    default:
      return QString();
  }
}

QDateTime TextFactory::parseDateTime(qint64 milis_from_epoch) {
  return QDateTime::fromMSecsSinceEpoch(milis_from_epoch);
}
