// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef PREDEFINEDTOOLS_H
#define PREDEFINEDTOOLS_H

#include <QString>

class PredefinedTools {
  public:
    static QString toBase64(QString data);
    static QString fromBase64(QString data);
    static QString toBase64Url(QString data);
    static QString fromBase64Url(QString data);

  private:
    explicit PredefinedTools();
};

#endif // PREDEFINEDTOOLS_H
