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

    bool isMacroNameUniqueNonEmpty(const QString& macro_name);

    Macro* recordedMacro() const;

    QList<Macro*> storedMacros() const;

    bool saveMacroAs(Macro* macro);
    void deleteMacro(Macro* macro);
    void addMacro(Macro* macro);
    void clearAllMacros();
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
