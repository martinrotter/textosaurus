// For license of this file, see <project-root-folder>/LICENSE.md.

#include "saurus/plugin-system/macros/macros.h"

#include "3rd-party/scintilla/qt/ScintillaEdit/ScintillaEdit.h"
#include "common/miscellaneous/settings.h"
#include "saurus/plugin-system/macros/macro.h"

Macros::Macros(Settings* settings, QObject* parent) : QObject(parent), m_settings(settings) {}

Macros::~Macros() {
  clearAllMacros();
}

void Macros::addMacro(Macro* macro) {
  macro->setParent(this);
  m_storedMacros.append(macro);
}

void Macros::clearAllMacros() {
  qDeleteAll(m_storedMacros);
  m_storedMacros.clear();
}

void Macros::recordNewMacro(ScintillaEdit* editor) {
  stopMacroRecording();

  if (m_recordingMacro != nullptr) {
    m_recordingMacro->deleteLater();
  }

  m_recordingMacro = new Macro(this);

  connect(m_recordingMacro, &Macro::newStepRecorded, this, &Macros::newStepRecorded);

  m_recordingMacro->startRecording(editor);
  emit recordingStarted();
}

void Macros::stopMacroRecording() {
  if (m_recordingMacro != nullptr) {
    m_recordingMacro->endRecording();
    m_recordingMacro->disconnect(this);

    emit recordingFinished(m_recordingMacro);
  }
}

void Macros::loadMacrosFromSettings() {
  clearAllMacros();

  // TODO: dodělat
}

Macro* Macros::recordedMacro() const {
  return m_recordingMacro;
}
