// For license of this file, see <project-root-folder>/LICENSE.md.

#include "external-tools/externaltools.h"

#include "external-tools/externaltool.h"
#include "miscellaneous/textapplication.h"

#include <QAction>
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
  }

  return actions;
}

const QList<ExternalTool*> ExternalTools::tools() const {
  return m_tools;
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
}
