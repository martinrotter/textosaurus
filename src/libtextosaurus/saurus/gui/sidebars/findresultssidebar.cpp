// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "saurus/gui/sidebars/findresultssidebar.h"

#include "saurus/gui/sidebars/findresultsmodel.h"
#include "saurus/gui/sidebars/findresultsmodelitem.h"
#include "saurus/gui/sidebars/findresultsmodelitemresult.h"
#include "saurus/miscellaneous/textapplication.h"

#include <QTreeView>

FindResultsSidebar::FindResultsSidebar(TextApplication* app, QWidget* parent)
  : BaseSidebar(app, parent), m_viewResults(nullptr), m_model(nullptr) {
  setWindowTitle(tr("Find Results"));
  setObjectName(QSL("m_sidebarFindResults"));
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

void FindResultsSidebar::load() {
  if (m_model == nullptr) {
    m_model = new FindResultsModel(this);

    m_viewResults = new QTreeView(this);
    m_viewResults->setHeaderHidden(true);
    m_viewResults->setModel(m_model);
    m_viewResults->setIndentation(10);
    m_viewResults->setAlternatingRowColors(true);
    m_viewResults->setItemDelegate(new HtmlDelegate(m_viewResults));

    setWidget(m_viewResults);
    connect(m_viewResults, &QTreeView::activated, this, &FindResultsSidebar::navigateToResult);

    BaseSidebar::load();
  }
}

void FindResultsSidebar::addResults(TextEditor* editor, const QList<QPair<int, int>> results) {
  show();
  raise();

  m_model->addResults(editor, results);
  m_viewResults->expand(m_model->index(0, 0, QModelIndex()));
}

void FindResultsSidebar::navigateToResult(const QModelIndex& index) {
  FindResultsModelItem* item = m_model->itemForIndex(index);
  FindResultsModelItemResult* result_item = qobject_cast<FindResultsModelItemResult*>(item);

  if (result_item != nullptr && result_item->editor() != nullptr) {
    m_textApp->makeEditorVisible(result_item->editor());

    result_item->editor()->ensureVisible(result_item->editor()->lineFromPosition(result_item->range().first));
    result_item->editor()->setSel(result_item->range().first, result_item->range().second);
  }
}
