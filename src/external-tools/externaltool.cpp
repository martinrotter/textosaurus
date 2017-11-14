// For license of this file, see <project-root-folder>/LICENSE.md.

#include "external-tools/externaltool.h"

#include "gui/texteditor.h"
#include "gui/toolbox.h"
#include "miscellaneous/iofactory.h"
#include "miscellaneous/textapplication.h"
#include "miscellaneous/textapplicationsettings.h"

#include <QProcess>

ExternalTool::ExternalTool(QObject* parent) : QObject(parent), m_input(ToolInput::SelectionDocument),
  m_output(ToolOutput::ReplaceSelectionDocument), m_shortcut(QString()), m_script(QString()), m_category(QString()),
  m_name(QString()) {}

ExternalTool::ExternalTool(const ExternalTool& other, QObject* parent) : QObject(parent) {
  setCategory(other.category());
  setId(other.id());
  setInput(other.input());
  setOutput(other.output());
  setName(other.name());
  setScript(other.script());
  setShortcut(other.shortcut());
}

bool ExternalTool::isPredefined() const {
  return false;
}

QPair<QString, bool> ExternalTool::runTool(const QPointer<TextEditor>& editor, const QString& data) {
  Q_UNUSED(editor)
  Q_UNUSED(data)

  // TODO: celkově dodělat efektivně.

  // TODO: Save script to file and make it executable.
  QString script_file = IOFactory::writeToTempFile(script().toUtf8());

  // TODO: Run bash "script.sh".
  // TODO: Get result.
  QProcess bash_process;

  bash_process.setProcessEnvironment(QProcessEnvironment::systemEnvironment());
  bash_process.setProcessChannelMode(QProcess::ProcessChannelMode::MergedChannels);
  bash_process.start(QSL("c:\\cygwin\\bin\\bash.exe"), QStringList() << script_file);

  if (bash_process.waitForFinished()) {
    QByteArray tool_output = bash_process.readAll();

    return QPair<QString, bool>(QString::fromUtf8(tool_output), true);
  }
  else {
    return QPair<QString, bool>(bash_process.errorString(), false);
  }
}

QString ExternalTool::id() const {
  return m_id;
}

void ExternalTool::setId(const QString& id) {
  m_id = id;
}

QString ExternalTool::shortcut() const {
  return m_shortcut;
}

void ExternalTool::setShortcut(const QString& shortcut) {
  m_shortcut = shortcut;
}

QString ExternalTool::category() const {
  return m_category;
}

void ExternalTool::setCategory(const QString& category) {
  m_category = category;
}

ToolOutput ExternalTool::output() const {
  return m_output;
}

void ExternalTool::setOutput(const ToolOutput& output) {
  m_output = output;
}

ToolInput ExternalTool::input() const {
  return m_input;
}

void ExternalTool::setInput(const ToolInput& input) {
  m_input = input;
}

QString ExternalTool::script() const {
  return m_script;
}

void ExternalTool::setScript(const QString& script) {
  m_script = script;
}

QString ExternalTool::name() const {
  return m_name;
}

void ExternalTool::setName(const QString& name) {
  m_name = name;
}

PredefinedTool::PredefinedTool(std::function<QString(const QString&, bool*)> functor, QObject* parent)
  : ExternalTool(parent), m_functor(functor) {}

QPair<QString, bool> PredefinedTool::runTool(const QPointer<TextEditor>& editor, const QString& data) {
  Q_UNUSED(editor)

  bool ok = true;
  QString result = m_functor(data, &ok);

  return QPair<QString, bool>(result, ok);
}

bool PredefinedTool::isPredefined() const {
  return true;
}
