// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifndef TEXTFACTORY_H
#define TEXTFACTORY_H

#include "definitions/definitions.h"

#include <QDateTime>
#include <QFontMetrics>

class QMenu;

class TextFactory {
  public:
    TextFactory() = delete;
    ~TextFactory() = delete;

    static int stringHeight(const QString& string, const QFontMetrics& metrics);
    static int stringWidth(const QString& string, const QFontMetrics& metrics);

    // Tries to parse input textual date/time representation.
    // Returns invalid date/time if processing fails.
    // NOTE: This method tries to always return time in UTC+00:00.
    static QDateTime parseDateTime(const QString& date_time);
    static void initializeEncodingMenu(QMenu* const menu, bool checkable = false);
    static QByteArray detectEncoding(const QByteArray& file_head_chunk);
    static int detectEol(const QByteArray& file_head_chunk);
    static QString eolDescriptionFromCode(int eol_code);

    // Converts 1970-epoch miliseconds to date/time.
    // NOTE: This apparently returns date/time in localtime.
    static QDateTime parseDateTime(qint64 milis_from_epoch);
};

#endif // TEXTFACTORY_H
