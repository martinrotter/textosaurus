// For license of this file, see <project-root-folder>/LICENSE.md.

#include "saurus/miscellaneous/macro.h"

#include "definitions/definitions.h"

Macro::Macro(const QString& saved_form, QObject* parent) : Macro(parent) {
  fromString(saved_form);
}

Macro::Macro(QObject* parent) : QObject(parent) {}

QString Macro::name() const {
  return m_name;
}

void Macro::setName(const QString& name) {
  m_name = name;
}

QString Macro::toString() const {
  return "";
}

void Macro::clear() {
  m_macroSteps.clear();
}

void Macro::play(ScintillaEdit* editor) {
  m_attachedEditor = editor;

  if (editor != nullptr) {
    for (const MacroStep& stp : m_macroSteps) {
      m_attachedEditor->send(static_cast<unsigned int>(stp.m_msg), stp.m_wParam, reinterpret_cast<sptr_t>(stp.m_text.data()));
    }
  }
}

void Macro::startRecording(ScintillaEdit* editor) {
  endRecording();
  clear();

  m_attachedEditor = editor;

  if (editor != nullptr) {
    connect(m_attachedEditor.data(), &ScintillaEdit::macroRecord, this, &Macro::recordStep);
    m_attachedEditor->send(SCI_STARTRECORD);
  }
}

void Macro::endRecording() {
  if (!m_attachedEditor.isNull()) {
    m_attachedEditor->disconnect(this);
    m_attachedEditor->send(SCI_STOPRECORD);
    m_attachedEditor.clear();
  }
}

void Macro::recordStep(int msg, uptr_t wParam, sptr_t lParam) {
  MacroStep m;

  m.m_msg = msg;
  m.m_cmd = static_cast<ScintillaCommand>(msg);
  m.m_wParam = wParam;

  // Special handling of some commands.
  switch (msg) {
    case SCI_ADDTEXT:
      m.m_text = QByteArray(reinterpret_cast<const char*>(lParam), int(wParam));
      break;

    case SCI_REPLACESEL:
      if (!m_macroSteps.isEmpty() && m_macroSteps.last().m_msg == SCI_REPLACESEL) {
        // This is the command used for ordinary user input so it's a
        // significant space reduction to append it to the previous
        // command.

        m_macroSteps.last().m_text.append(reinterpret_cast<const char*>(lParam));
        return;
      }

      [[fallthrough]];

    case SCI_INSERTTEXT:
    case SCI_APPENDTEXT:
    case SCI_SEARCHNEXT:
    case SCI_SEARCHPREV:
      m.m_text.append(reinterpret_cast<const char*>(lParam));
      break;
  }

  m_macroSteps.append(m);
  emit newStepRecorded(m);
}

void Macro::fromString(const QString& asc) {
  clear();
}

QList<Macro::MacroStep> Macro::macroSteps() const {
  return m_macroSteps;
}
