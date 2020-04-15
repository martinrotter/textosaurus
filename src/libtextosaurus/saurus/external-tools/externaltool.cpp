// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "saurus/external-tools/externaltool.h"

#include "common/miscellaneous/iofactory.h"
#include "saurus/gui/sidebars/outputsidebar.h"
#include "saurus/gui/texteditor.h"
#include "saurus/miscellaneous/textapplication.h"
#include "saurus/miscellaneous/textapplicationsettings.h"

#include <QAction>
#include <QDir>
#include <QPointer>
#include <QProcess>

ExternalTool::ExternalTool(QObject* parent) : QObject(parent), m_isRunning(false), m_addToEditMenu(false),
  m_actionObjectName(QString()), m_action(nullptr), m_input(ToolInput::SelectionDocument),
  m_output(ToolOutput::ReplaceSelectionDocument), m_prompt(QString()), m_shortcut(QString()), m_category(QString()),
  m_name(QString()), m_interpreter(EXT_TOOL_INTERPRETER), m_script(QString()) {}

ExternalTool::ExternalTool(const ExternalTool& other, QObject* parent) : ExternalTool(parent) {
  setCategory(other.category());
  setInput(other.input());
  setOutput(other.output());
  setName(other.name());
  setInterpreter(other.interpreter());
  setScript(other.script());
  setShortcut(other.shortcut());
  setPrompt(other.prompt());
  setAddToEditMenu(other.addToEditMenu());
  setActionObjectName(other.actionObjectName());

  // NOTE: Not copying m_action.
}

bool ExternalTool::isPredefined() const {
  return false;
}

void ExternalTool::runTool(QPointer<TextEditor> editor, const QString& data) {
  Q_UNUSED(editor)

  m_isRunning = true;

  // Save script to file and make it executable.
  QString script_file = IOFactory::writeToTempFile(script().toUtf8());
  QString work_script_directory = QDir::toNativeSeparators(QFileInfo(script_file).canonicalPath());

  // Now make script file a relative filename with leading ".\" or "./".
  script_file = QL1C('.') + QDir::separator() + QFileInfo(script_file).fileName();

  // Run in interpreter.
  auto* script_process = new QProcess(this);
  auto env = QProcessEnvironment::systemEnvironment();

  script_process->setWorkingDirectory(work_script_directory);

  // Add a set of variables to have info about some
  // Textosaurus-related properties.
  env.insert("SAURUS_DOCUMENT_PATH", QDir::toNativeSeparators(editor->filePath()));
  env.insert("SAURUS_DOCUMENT_FOLDER", QDir::toNativeSeparators(QFileInfo(editor->filePath()).canonicalPath()));
  env.insert("SAURUS_DOCUMENT_FILE", QDir::toNativeSeparators(QFileInfo(editor->filePath()).fileName()));

  script_process->setProcessEnvironment(env);

  connect(script_process, &QProcess::readyReadStandardOutput, this, [this, script_process]() {
    if (m_output == ToolOutput::DumpToOutputWindow) {
      QString available_output = QString::fromUtf8(script_process->readAllStandardOutput());

      if (!available_output.isEmpty()) {
        emit partialOutputObtained(available_output);
      }
    }
  });

  connect(script_process, &QProcess::started, this, [data, script_process] {
    script_process->write(data.toUtf8());
    script_process->closeWriteChannel();
  });

  connect(script_process, &QProcess::errorOccurred, this, [editor, this] {
    onProcessError(editor);
  });

  connect(script_process, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
          this, [editor, this](int exit_code, QProcess::ExitStatus exit_status) {
    onProcessFinished(editor, exit_code, exit_status);
  });

#if defined (Q_OS_WIN)
  QString interp = interpreter();

  if (!interp.endsWith(QL1S(".exe"))) {
    interp += QSL(".exe");
  }

  script_process->start(interp, QStringList() << script_file);
#else
  script_process->start(interpreter(), QStringList() << script_file);
#endif
}

void ExternalTool::onProcessError(QPointer<TextEditor> editor) {
  auto* bash_process = qobject_cast<QProcess*>(sender());

  if (bash_process != nullptr) {
    emit toolFinished(editor, QString(), bash_process->errorString(), false);

    bash_process->deleteLater();
  }

  m_isRunning = false;
}

void ExternalTool::onProcessFinished(QPointer<TextEditor> editor, int exit_code, QProcess::ExitStatus exit_status) {
  auto* script_process = qobject_cast<QProcess*>(sender());

  if (script_process != nullptr) {
    if (exit_status == QProcess::ExitStatus::NormalExit) {
      QByteArray tool_output = script_process->readAllStandardOutput();
      QByteArray tool_error = script_process->readAllStandardError();
      emit toolFinished(editor, QString::fromUtf8(tool_output), QString::fromUtf8(tool_error), exit_code == 0);
    }
    else {
      emit toolFinished(editor, QString(), script_process->errorString(), false);
    }

    script_process->deleteLater();
  }

  m_isRunning = false;
}

QString ExternalTool::promptValue() const {
  return m_promptValue;
}

void ExternalTool::setPromptValue(const QString& prompt_value) {
  m_promptValue = prompt_value;
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

ExternalTool::ToolOutput ExternalTool::output() const {
  return m_output;
}

void ExternalTool::setOutput(const ToolOutput& output) {
  m_output = output;
}

ExternalTool::ToolInput ExternalTool::input() const {
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

QAction* ExternalTool::action() const {
  return m_action;
}

void ExternalTool::setAction(QAction* action) {
  m_action = action;
}

QString ExternalTool::name() const {
  return m_name;
}

void ExternalTool::setName(const QString& name) {
  m_name = name;
}

bool ExternalTool::addToEditMenu() const {
  return m_addToEditMenu;
}

void ExternalTool::setAddToEditMenu(bool add_to_edit_menu) {
  m_addToEditMenu = add_to_edit_menu;
}

QString ExternalTool::actionObjectName() const {
  return m_actionObjectName;
}

void ExternalTool::setActionObjectName(const QString& action_obj_name) {
  m_actionObjectName = action_obj_name;
}

PredefinedTool::PredefinedTool(std::function<QString(const QString&, bool&)> functor, QObject* parent)
  : ExternalTool(parent), m_functor(std::move(functor)) {}

void PredefinedTool::runTool(QPointer<TextEditor> editor, const QString& data) {
  Q_UNUSED(editor)

  bool ok = true;
  QString result = m_functor(data, ok);
  emit toolFinished(editor, ok ? result : QString(), ok ? QString() : result, ok);
}

bool PredefinedTool::isPredefined() const {
  return true;
}
