// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef PREDEFINEDTOOLS_H
#define PREDEFINEDTOOLS_H

#include <QString>

class PredefinedTools {
  public:

    // Uploading.
    static QString sendToClbin(const QString& data, bool* ok);
    static QString sendToIxio(const QString& data, bool* ok);
    static QString sendToSprunge(const QString& data, bool* ok);
    static QString sendToGithub(const QString& data, bool* ok);

    // Inserting.
    static QString currentDateTime(const QString& data, bool* ok);

    // MIME tools.
    static QString toBase64(const QString& data, bool* ok);
    static QString fromBase64(const QString& data, bool* ok);
    static QString toBase64Url(const QString& data, bool* ok);
    static QString fromBase64Url(const QString& data, bool* ok);

  private:
    explicit PredefinedTools();
};

#endif // PREDEFINEDTOOLS_H
