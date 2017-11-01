// For license of this file, see <project-root-folder>/LICENSE.md.

#include "external-tools/externaltool.h"

ExternalTool::ExternalTool(QObject* parent) : QObject(parent) {}

bool ExternalTool::isPredefined() const {
  return false;
}

void ExternalTool::runTool(const QString& application_path, const QString& file_path, const QString& selected_text) {}

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

PredefinedTool::PredefinedTool(std::function<QString (const QString&, const QString&, const QString&)> functor, QObject* parent)
  : QObject(parent), m_functor(functor) {}

void PredefinedTool::runTool(const QString& application_path, const QString& file_path, const QString& selected_text) {
  QString result = m_functor(application_path, file_path, selected_text);
}

bool PredefinedTool::isPredefined() const {
  return true;
}
