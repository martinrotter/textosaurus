// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef MACROS_H
#define MACROS_H

#include <QObject>

#include "common/miscellaneous/settings.h"
#include "saurus/miscellaneous/macro.h"

class Macros : public QObject {
  Q_OBJECT

  public:
    explicit Macros(QObject* parent = nullptr);
    virtual ~Macros();

  public slots:
    void addMacro(Macro* macro);
    void clearAllMacros();

  private:
    void loadMacrosFromSettings(const Settings* settings);

  signals:
    void recordingStarted();
    void recordingFinished(Macro* finished_macro);

  private:
    QList<Macro*> m_storedMacros = QList<Macro*>();

    Macro* m_recordingMacro;
};

#endif // MACROS_H
