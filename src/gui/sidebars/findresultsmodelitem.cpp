// For license of this file, see <project-root-folder>/LICENSE.md.

#include "findresultsmodelitem.h"

#include <QVariant>

FindResultsModelItem::FindResultsModelItem(QObject* parent)
  : QObject(parent), m_childItems(QList<FindResultsModelItem*>()), m_parentItem(nullptr) {}

FindResultsModelItem::~FindResultsModelItem() {
  qDeleteAll(m_childItems);
}

QVariant FindResultsModelItem::data() const {
  return "aaa";
}

void FindResultsModelItem::appendChild(FindResultsModelItem* item) {
  m_childItems.append(item);
  item->setParentItem(this);
  item->setParent(this);
}

FindResultsModelItem* FindResultsModelItem::child(int row) {
  return m_childItems.value(row);
}

int FindResultsModelItem::childCount() const {
  return m_childItems.count();
}

FindResultsModelItem* FindResultsModelItem::parentItem() {
  return m_parentItem;
}

void FindResultsModelItem::setParentItem(FindResultsModelItem* parent_item) {
  m_parentItem = parent_item;
}

int FindResultsModelItem::row() const {
  if (m_parentItem != nullptr) {
    return m_parentItem->m_childItems.indexOf(const_cast<FindResultsModelItem*>(this));
  }
  else {
    return 0;
  }
}
