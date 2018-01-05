// For license of this file, see <project-root-folder>/LICENSE.md.

#include "gui/sidebars/findresultssidebar.h"

#include <QTreeWidget>

FindResultsSidebar::FindResultsSidebar(TextApplication* app, QWidget* parent)
  : DockWidget(parent), m_viewResults(new QTreeWidget(this)) {
  setWidget(m_viewResults);
  setWindowTitle(tr("Find Results"));

  m_viewResults->setHeaderHidden(true);
}

Qt::DockWidgetArea FindResultsSidebar::initialArea() const {
  return Qt::DockWidgetArea::BottomDockWidgetArea;
}

bool FindResultsSidebar::initiallyVisible() const {
  return false;
}

int FindResultsSidebar::initialWidth() const {
  return 100;
}

void FindResultsSidebar::load() {}

void FindResultsSidebar::addResults(TextEditor* editor, const QList<QPair<int, int>> results) {}
