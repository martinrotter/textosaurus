// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef MACROS_H
#define MACROS_H

#include <QObject>

#include "saurus/plugin-system/macros/macro.h"

class Settings;
class ScintillaEdit;

class Macros : public QObject {
  Q_OBJECT

  public:
    explicit Macros(Settings* settings, QObject* parent = nullptr);
    virtual ~Macros();

    Macro* recordedMacro() const;

  public slots:
    void addMacro(Macro* macro);
    void clearAllMacros();
    void saveMacroAs(Macro* macro);

    void recordNewMacro(ScintillaEdit* editor);
    void stopMacroRecording();

  private:
    void loadMacrosFromSettings();
    void sortStoredMacros();

  signals:
    void recordingStarted();
    void newStepRecorded(Macro::MacroStep macro_step);
    void recordingFinished(Macro* finished_macro);

  private:
    Settings* m_settings;

    QList<Macro*> m_storedMacros = QList<Macro*>();

    Macro* m_recordingMacro = nullptr;
};

#endif // MACROS_H
