// For license of this file, see <project-root-folder>/LICENSE.md.

#include "gui/sidebars/findresultssidebar.h"

#include "gui/sidebars/findresultsmodel.h"
#include "gui/sidebars/findresultsmodelitem.h"
#include "gui/sidebars/findresultsmodelitemresult.h"
#include "miscellaneous/textapplication.h"

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

  connect(m_viewResults, &QTreeView::activated, this, &FindResultsSidebar::navigateToResult);
}

void FindResultsSidebar::addResults(TextEditor* editor, const QList<QPair<int, int>> results) {
  show();
  raise();

  m_model->addResults(editor, results);
}

void FindResultsSidebar::navigateToResult(const QModelIndex& index) {
  FindResultsModelItem* item = m_model->itemForIndex(index);
  FindResultsModelItemResult* result_item = qobject_cast<FindResultsModelItemResult*>(item);

  if (result_item != nullptr && result_item->editor() != nullptr) {
    m_textApp->makeEditorVisible(result_item->editor());

    result_item->editor()->setSel(result_item->range().first, result_item->range().second);
  }
}
