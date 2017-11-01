// For license of this file, see <project-root-folder>/LICENSE.md.

#include "external-tools/externaltools.h"

#include "external-tools/externaltool.h"

#include <QAction>

ExternalTools::ExternalTools(QObject* parent) : QObject(parent), m_tools(QList<ExternalTool*>()) {}

ExternalTools::~ExternalTools() {
  qDeleteAll(m_tools);
}

QList<QAction*> ExternalTools::generateActions() const {
  QList<QAction*> actions;

  foreach (ExternalTool* tool, m_tools) {
    QAction* act = new QAction(tool->name());

    act->setData(tool->id());

  }

  return actions;
}

const QList<ExternalTool*> ExternalTools::tools() const {
  return m_tools;
}
