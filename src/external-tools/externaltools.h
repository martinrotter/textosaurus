// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef EXTERNALTOOLS_H
#define EXTERNALTOOLS_H

#include <QObject>

// Top-level management class for external tools.
class ExternalTools : public QObject {
  Q_OBJECT

  public:
    explicit ExternalTools(QObject* parent = nullptr);

  signals:

  public slots:
};

#endif // EXTERNALTOOLS_H
