// For license of this file, see <project-root-folder>/LICENSE.md.

#include "external-tools/externaltool.h"

#include "gui/texteditor.h"
#include "gui/outputwindow.h"
#include "miscellaneous/iofactory.h"
#include "miscellaneous/textapplication.h"
#include "miscellaneous/textapplicationsettings.h"

#include <QProcess>

ExternalTool::ExternalTool(QObject* parent) : QObject(parent), m_isRunning(false), m_input(ToolInput::SelectionDocument),
  m_output(ToolOutput::ReplaceSelectionDocument), m_prompt(QString()), m_shortcut(QString()), m_category(QString()),
  m_name(QString()), m_interpreter(EXT_TOOL_INTERPRETER), m_script(QString()) {}

ExternalTool::ExternalTool(const ExternalTool& other, QObject* parent) : QObject(parent) {
  setCategory(other.category());
  setInput(other.input());
  setOutput(other.output());
  setName(other.name());
  setInterpreter(other.interpreter());
  setScript(other.script());
  setShortcut(other.shortcut());
  setPrompt(other.prompt());
}

bool ExternalTool::isPredefined() const {
  return false;
}

QPair<QString, bool> ExternalTool::runTool(const QPointer<TextEditor>& editor, const QString& data) {
  Q_UNUSED(editor)

  m_isRunning = true;

  // Save script to file and make it executable.
  QString script_file = IOFactory::writeToTempFile(script().toUtf8());

  // Run in bash.
  QProcess bash_process;

  bash_process.setProcessEnvironment(QProcessEnvironment::systemEnvironment());
  bash_process.setProcessChannelMode(QProcess::ProcessChannelMode::MergedChannels);

#if defined (Q_OS_WIN)
  QString intepr = interpreter();

  if (!intepr.endsWith(QL1S(".exe"))) {
    intepr += QSL(".exe");
  }

  bash_process.start(intepr, QStringList() << script_file);
#else
  bash_process.start(interpreter(), QStringList() << script_file);
#endif

  bash_process.write(data.toUtf8());
  bash_process.closeWriteChannel();

  if (bash_process.waitForFinished(1000 * 120)) {
    m_isRunning = false;

    // Get result.
    QByteArray tool_output = bash_process.readAll();

    return QPair<QString, bool>(QString::fromUtf8(tool_output), bash_process.exitCode() == 0);
  }
  else {
    m_isRunning = false;

    bash_process.kill();
    return QPair<QString, bool>(bash_process.errorString(), false);
  }
}

bool ExternalTool::isRunning() const {
  return m_isRunning;
}

QString ExternalTool::prompt() const {
  return m_prompt;
}

void ExternalTool::setPrompt(const QString& prompt) {
  m_prompt = prompt;
}

QString ExternalTool::interpreter() const {
  return m_interpreter;
}

void ExternalTool::setInterpreter(const QString& interpreter) {
  m_interpreter = interpreter;
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
