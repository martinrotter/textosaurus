// For license of this file, see <project-root-folder>/LICENSE.md.

#include "external-tools/externaltool.h"

#include "gui/texteditor.h"
#include "gui/toolbox.h"
#include "miscellaneous/textapplication.h"
#include "miscellaneous/textapplicationsettings.h"

ExternalTool::ExternalTool(QObject* parent) : QObject(parent) {}

bool ExternalTool::isPredefined() const {
  return false;
}

void ExternalTool::runTool(const QPointer<TextEditor>& editor) {
  if (!editor.isNull()) {
    QMetaObject::invokeMethod(editor->textApplication()->toolBox(), SLOT(displayOutput(OutputSource,QString)),
                              Qt::AutoConnection,
                              Q_ARG(OutputSource, OutputSource::ExternalTool),
                              Q_ARG(QString, "Running tool"));
  }
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

QString ExternalTool::command() const {
  return m_command;
}

void ExternalTool::setCommand(const QString& command) {
  m_command = command;
}

QString ExternalTool::id() const {
  return m_id;
}

void ExternalTool::setId(const QString& id) {
  m_id = id;
}

QString ExternalTool::name() const {
  return m_name;
}

void ExternalTool::setName(const QString& name) {
  m_name = name;
}

PredefinedTool::PredefinedTool(std::function<QString(const QString&)> functor, QObject* parent)
  : ExternalTool(parent), m_functor(functor) {}

void PredefinedTool::runTool(const QPointer<TextEditor>& editor) {
  QString result;

  if (!editor.isNull()) {
    QMetaObject::invokeMethod(editor->textApplication()->toolBox(), SLOT(displayOutput(OutputSource,QString)),
                              Qt::AutoConnection,
                              Q_ARG(OutputSource, OutputSource::ExternalTool),
                              Q_ARG(QString, "Running predefined tool"));
  }

  //m_functor(application_path, file_path, selected_text);
}

bool PredefinedTool::isPredefined() const {
  return true;
}
