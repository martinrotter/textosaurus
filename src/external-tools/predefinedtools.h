// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef PREDEFINEDTOOLS_H
#define PREDEFINEDTOOLS_H

#include <QString>

class PredefinedTools {
  public:

    // Uploading.
    static QString sendToClbin(const QString& data);
    static QString sendToIxio(const QString& data);
    static QString sendToSprunge(const QString& data);
    static QString sendToGithub(const QString& data);

    // Inserting.
    static QString currentDateTime(const QString& data);

    // MIME tools.
    static QString toBase64(const QString& data);
    static QString fromBase64(const QString& data);
    static QString toBase64Url(const QString& data);
    static QString fromBase64Url(const QString& data);

  private:
    explicit PredefinedTools();
};

#endif // PREDEFINEDTOOLS_H
