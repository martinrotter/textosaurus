// For license of this file, see <project-root-folder>/LICENSE.md.

#include "gui/sidebars/findresultsmodel.h"

#include "gui/sidebars/findresultsmodelitem.h"
#include "gui/sidebars/findresultsmodelitemeditor.h"
#include "gui/sidebars/findresultsmodelitemresult.h"

FindResultsModel::FindResultsModel(QObject* parent)
  : QAbstractItemModel(parent), m_rootItem(new FindResultsModelItem(this)) {

  auto aa = new FindResultsModelItemEditor(this);
  auto bb = new FindResultsModelItemResult(this);
  auto cc = new FindResultsModelItemResult(this);

  aa->appendChild(bb);
  aa->appendChild(cc);
  m_rootItem->appendChild(aa);
}

FindResultsModel::~FindResultsModel() {
  m_rootItem->deleteLater();
}

QModelIndex FindResultsModel::index(int row, int column, const QModelIndex& parent) const {
  if (!hasIndex(row, column, parent)) {
    return QModelIndex();
  }

  FindResultsModelItem* parentItem = itemForIndex(parent);
  FindResultsModelItem* childItem = parentItem->child(row);

  if (childItem) {
    return createIndex(row, column, childItem);
  }
  else {
    return QModelIndex();
  }
}

QModelIndex FindResultsModel::parent(const QModelIndex& child) const {
  if (!child.isValid()) {
    return QModelIndex();
  }

  FindResultsModelItem* childItem = itemForIndex(child);
  FindResultsModelItem* parentItem = childItem->parentItem();

  if (parentItem == m_rootItem) {
    return QModelIndex();
  }
  else {
    return createIndex(parentItem->row(), 0, parentItem);
  }
}

int FindResultsModel::rowCount(const QModelIndex& parent) const {
  if (parent.column() > 0) {
    return 0;
  }

  FindResultsModelItem* parentItem = itemForIndex(parent);

  return parentItem->childCount();
}

int FindResultsModel::columnCount(const QModelIndex& parent) const {
  Q_UNUSED(parent)
  return 1;
}

QVariant FindResultsModel::data(const QModelIndex& index, int role) const {
  if (!index.isValid()) {
    return QVariant();
  }
  else {
    return itemForIndex(index)->data(role);
  }
}

FindResultsModelItem* FindResultsModel::itemForIndex(const QModelIndex& idx) const {
  if (idx.isValid() && idx.model() == this) {
    return static_cast<FindResultsModelItem*>(idx.internalPointer());
  }
  else {
    return m_rootItem;
  }
}
