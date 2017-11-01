// For license of this file, see <project-root-folder>/LICENSE.md.

#include "external-tools/externaltools.h"

#include "external-tools/externaltool.h"
#include "miscellaneous/textapplication.h"

#include <functional>

#include <QAction>
#include <QDateTime>
#include <QPointer>

ExternalTools::ExternalTools(TextApplication* parent)
  : QObject(parent), m_textApplication(parent), m_tools(QList<ExternalTool*>()) {}

ExternalTools::~ExternalTools() {
  qDeleteAll(m_tools);
  qDebug("Destroying ExternalTools.");
}

QList<QAction*> ExternalTools::generateActions() const {
  QList<QAction*> actions;

  foreach (ExternalTool* tool, m_tools) {
    QAction* act = new QAction(tool->name());

    act->setData(QVariant::fromValue(tool));

    connect(act, &QAction::triggered, this, &ExternalTools::runTool);
    connect(tool, &ExternalTool::toolFinished, this, &ExternalTools::onToolFinished);

    actions.append(act);
  }

  return actions;
}

const QList<ExternalTool*> ExternalTools::tools() const {
  return m_tools;
}

void ExternalTools::reloadTools() {
  qDeleteAll(m_tools);
  m_tools.clear();

  std::function<QString(const QString&)> get_date = [](const QString& data) {
                                                      return QDateTime::currentDateTime().toString(Qt::DateFormat::ISODate);
                                                    };

  PredefinedTool* predefined_inset_date_time = new PredefinedTool(get_date, this);

  predefined_inset_date_time->setName(tr("Insert date/time"));
  predefined_inset_date_time->setId(QSL("insert-date-time"));
  predefined_inset_date_time->setInput(ToolInput::NoInput);
  predefined_inset_date_time->setOutput(ToolOutput::InsertAtCursorPosition);

  m_tools.append(predefined_inset_date_time);

  emit externalToolsChanged(generateActions());
}

void ExternalTools::runTool() {
  ExternalTool* tool_to_run = qobject_cast<QAction*>(sender())->data().value<ExternalTool*>();

  if (m_textApplication->currentEditor() != nullptr) {
    QPointer<TextEditor> ptr_editor = m_textApplication->currentEditor();

    // TODO: případně uložíme tento editor, a zavoláme akci.

    tool_to_run->runTool(ptr_editor);
  }
}

void ExternalTools::onToolFinished(QPointer<TextEditor> editor, const QString& output_text) {
  ExternalTool* tool = qobject_cast<ExternalTool*>(sender());

  // TODO: we do something with the text.
  switch (tool->output()) {
    case ToolOutput::InsertAtCursorPosition:
      editor->insert(output_text);
      break;

    case ToolOutput::DumpToOutputWindow:
      break;

    case ToolOutput::NewSavedFile:
      break;

    case ToolOutput::ReplaceSelectionDocument:
      break;
  }
}
