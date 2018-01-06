// For license of this file, see <project-root-folder>/LICENSE.md.

#include "gui/sidebars/findresultssidebar.h"

#include "gui/sidebars/findresultsmodel.h"
#include "gui/sidebars/findresultsmodelitem.h"

#include <QTreeView>

FindResultsSidebar::FindResultsSidebar(TextApplication* app, QWidget* parent)
  : DockWidget(parent), m_textApp(app) {}

Qt::DockWidgetArea FindResultsSidebar::initialArea() const {
  return Qt::DockWidgetArea::BottomDockWidgetArea;
}

bool FindResultsSidebar::initiallyVisible() const {
  return false;
}

int FindResultsSidebar::initialWidth() const {
  return 100;
}

void FindResultsSidebar::load() {
  m_model = new FindResultsModel(this);

  m_viewResults = new QTreeView(this);
  m_viewResults->setHeaderHidden(true);
  m_viewResults->setModel(m_model);
  m_viewResults->setIndentation(10);

  setWidget(m_viewResults);
  setWindowTitle(tr("Find Results"));
}

void FindResultsSidebar::clear() {}

void FindResultsSidebar::addResults(TextEditor* editor, const QList<QPair<int, int>> results) {}
