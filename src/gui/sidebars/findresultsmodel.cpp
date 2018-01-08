// For license of this file, see <project-root-folder>/LICENSE.md.

#include "gui/sidebars/findresultsmodel.h"

#include "gui/sidebars/findresultsmodelitem.h"
#include "gui/sidebars/findresultsmodelitemeditor.h"
#include "gui/sidebars/findresultsmodelitemresult.h"

FindResultsModel::FindResultsModel(QObject* parent)
  : QAbstractItemModel(parent), m_rootItem(new FindResultsModelItem(this)) {}

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

  if (parentItem == m_rootItem.data()) {
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

void FindResultsModel::clear() {
  beginRemoveRows(QModelIndex(), 0, rowCount(QModelIndex()));
  m_rootItem->clearChildren();
  endRemoveRows();
}

void FindResultsModel::addResults(TextEditor* editor, const QList<QPair<int, int>> results) {
  clear();

  beginInsertRows(QModelIndex(), 0, 1);
  FindResultsModelItemEditor* item_editor = new FindResultsModelItemEditor(editor, this);

  for (const QPair<int, int> range : results) {
    QString text = editor->textRange(range.first, range.second);
    FindResultsModelItemResult* item_result = new FindResultsModelItemResult(text, range, item_editor);

    item_editor->appendChild(item_result);
  }

  m_rootItem->appendChild(item_editor);
  endInsertRows();
}

FindResultsModelItem* FindResultsModel::itemForIndex(const QModelIndex& idx) const {
  if (idx.isValid() && idx.model() == this) {
    return static_cast<FindResultsModelItem*>(idx.internalPointer());
  }
  else {
    return m_rootItem.data();
  }
}
