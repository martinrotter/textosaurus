// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef PREDEFINEDTOOLS_H
#define PREDEFINEDTOOLS_H

#include <QString>

class PredefinedTools {
  public:
    PredefinedTools() = delete;
    ~PredefinedTools() = delete;

    // Uploading.
    static QString sendToClbin(const QString& data, bool* ok);
    static QString sendToIxio(const QString& data, bool* ok);
    static QString sendToSprunge(const QString& data, bool* ok);
    static QString sendToGithub(const QString& data, bool* ok);

    // JSON.
    static QString jsonBeautify(const QString& data, bool* ok);
    static QString jsonMinify(const QString& data, bool* ok);

    // XML.
    static QString xmlCheck(const QString& data, bool* ok);
    static QString xmlBeautify(const QString& data, bool* ok);
    static QString xmlLinearize(const QString& data, bool* ok);

    // Date/time.
    static QString currentDateTime(const QString& data, bool* ok);
    static QString currentDate(const QString& data, bool* ok);
    static QString currentTime(const QString& data, bool* ok);
    static QString formattedDateTime(const QString& data, bool* ok);

    // Change case.
    static QString toLower(const QString& data, bool* ok);
    static QString toUpper(const QString& data, bool* ok);
    static QString toTitleCase(const QString& data, bool* ok);
    static QString toSentenceCase(const QString& data, bool* ok);
    static QString invertCase(const QString& data, bool* ok);

    // MIME tools.
    static QString toUrlEncoded(const QString& data, bool* ok);
    static QString fromUrlEncoded(const QString& data, bool* ok);
    static QString toBase64(const QString& data, bool* ok);
    static QString fromBase64(const QString& data, bool* ok);
    static QString toBase64Url(const QString& data, bool* ok);
    static QString fromBase64Url(const QString& data, bool* ok);
    static QString toHtmlEscaped(const QString& data, bool* ok);
};

#endif // PREDEFINEDTOOLS_H
