// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "saurus/plugin-system/macros/macros.h"

#include "definitions/definitions.h"

// *INDENT-OFF*
#include PATH(SCINTILLA_DIR,qt/ScintillaEdit/ScintillaEdit.h)

// *INDENT-ON*

#include "common/gui/messagebox.h"
#include "common/miscellaneous/settings.h"
#include "saurus/plugin-system/macros/macro.h"
#include "saurus/plugin-system/macros/macrosplugin.h"

#include <QInputDialog>

Macros::Macros(MacrosPlugin* plugin, QObject* parent) : QObject(parent), m_plugin(plugin) {
  loadMacrosFromSettings();
}

Macros::~Macros() {
  clearAllMacros();
}

bool Macros::isMacroNameUniqueNonEmpty(const QString& macro_name) {
  if (macro_name.isEmpty()) {
    return false;
  }
  else {
    for (const Macro* mac : m_storedMacros) {
      if (QString::compare(mac->name(), macro_name, Qt::CaseSensitivity::CaseInsensitive) == 0) {
        return false;
      }
    }

    return true;
  }
}

void Macros::addMacro(Macro* macro) {
  macro->setParent(this);
  m_storedMacros.append(macro);
}

void Macros::clearAllMacros() {
  if (m_recordingMacro != nullptr) {
    m_recordingMacro->deleteLater();
  }

  qDeleteAll(m_storedMacros);
  m_storedMacros.clear();
}

bool Macros::saveMacroAs(Macro* macro) {
  bool ok;
  QString macro_name = QInputDialog::getText(m_plugin->mainForm(), tr("Save Macro"),
                                             tr("Enter some name for your macro"), QLineEdit::EchoMode::Normal,
                                             tr("My new macro"), &ok);

  if (ok) {
    if (isMacroNameUniqueNonEmpty(macro_name)) {
      macro->setName(macro_name);
      auto str_representation = macro->toString();

      // Save macro to settings.
      m_plugin->settings()->setValue(GROUP(StoredMacros), macro->name(), str_representation);

      addMacro(macro);
      sortStoredMacros();
      return true;
    }
    else {
      MessageBox::show(m_plugin->mainForm(), QMessageBox::Icon::Critical, tr("Macro Name Already Used"),
                       tr("Selected macro name is already in use, please select non-empty unique name for each macro."),
                       QString(), macro_name);
    }
  }

  return false;
}

void Macros::deleteMacro(Macro* macro) {
  m_storedMacros.removeAll(macro);
  m_plugin->settings()->remove(GROUP(StoredMacros), macro->name());
}

void Macros::recordNewMacro(ScintillaEdit* editor) {
  stopMacroRecording();
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

  m_plugin->settings()->beginGroup(GROUP(StoredMacros));

  auto keys = m_plugin->settings()->allKeys();

  m_plugin->settings()->endGroup();

  for (const QString& key : keys) {
    auto macr = new Macro(m_plugin->settings()->value(GROUP(StoredMacros), key).toString());

    addMacro(macr);
  }

  sortStoredMacros();
}

void Macros::sortStoredMacros() {
  std::sort(m_storedMacros.begin(), m_storedMacros.end(), [](Macro* lhs, Macro* rhs) {
    return QString::compare(lhs->name(), rhs->name(), Qt::CaseSensitivity::CaseInsensitive) < 0;
  });
}

QList<Macro*> Macros::storedMacros() const {
  return m_storedMacros;
}

Macro* Macros::recordedMacro() const {
  return m_recordingMacro;
}
