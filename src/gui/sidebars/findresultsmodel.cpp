// For license of this file, see <project-root-folder>/LICENSE.md.

#include "gui/sidebars/findresultsmodel.h"

#include "gui/sidebars/findresultsmodelitem.h"

FindResultsModel::FindResultsModel(QObject* parent)
  : QAbstractItemModel(parent), m_rootItem(new FindResultsModelItem(this)) {

  auto aa = new FindResultsModelItem(this);
  auto bb = new FindResultsModelItem(this);
  auto cc = new FindResultsModelItem(this);

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

  FindResultsModelItem* parentItem;

  if (!parent.isValid()) {
    parentItem = m_rootItem;
  }
  else {
    parentItem = static_cast<FindResultsModelItem*>(parent.internalPointer());
  }

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

  FindResultsModelItem* childItem = static_cast<FindResultsModelItem*>(child.internalPointer());
  FindResultsModelItem* parentItem = childItem->parentItem();

  if (parentItem == m_rootItem) {
    return QModelIndex();
  }

  return createIndex(parentItem->row(), 0, parentItem);
}

int FindResultsModel::rowCount(const QModelIndex& parent) const {
  FindResultsModelItem* parentItem;

  if (parent.column() > 0) {
    return 0;
  }

  if (!parent.isValid()) {
    parentItem = m_rootItem;
  }
  else {
    parentItem = static_cast<FindResultsModelItem*>(parent.internalPointer());
  }

  return parentItem->childCount();
}

int FindResultsModel::columnCount(const QModelIndex& parent) const {
  return 1;
}

QVariant FindResultsModel::data(const QModelIndex& index, int role) const {
  if (!index.isValid()) {
    return QVariant();
  }

  if (role != Qt::DisplayRole) {
    return QVariant();
  }

  FindResultsModelItem* item = static_cast<FindResultsModelItem*>(index.internalPointer());

  return item->data();
}
