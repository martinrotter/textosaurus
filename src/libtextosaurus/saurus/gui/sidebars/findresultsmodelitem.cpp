// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "findresultsmodelitem.h"

#include <QVariant>

FindResultsModelItem::FindResultsModelItem(QObject* parent)
  : QObject(parent), m_parentItem(nullptr) {}

FindResultsModelItem::~FindResultsModelItem() {
  qDeleteAll(m_childItems);
}

QVariant FindResultsModelItem::data(int role) const {
  Q_UNUSED(role)
  return QVariant();
}

void FindResultsModelItem::appendChild(FindResultsModelItem* child) {
  m_childItems.append(child);
  child->setParentItem(this);
  child->setParent(this);
}

FindResultsModelItem* FindResultsModelItem::child(int row) {
  return m_childItems.value(row);
}

int FindResultsModelItem::childCount() const {
  return m_childItems.count();
}

FindResultsModelItem* FindResultsModelItem::parentItem() const {
  return m_parentItem;
}

void FindResultsModelItem::setParentItem(FindResultsModelItem* parent_item) {
  m_parentItem = parent_item;
}

QList<FindResultsModelItem*> FindResultsModelItem::childItems() const {
  return m_childItems;
}

void FindResultsModelItem::clearChildren() {
  for (FindResultsModelItem* child : m_childItems) {
    child->deleteLater();
  }

  m_childItems.clear();
}

int FindResultsModelItem::row() const {
  if (m_parentItem != nullptr) {
    return m_parentItem->m_childItems.indexOf(const_cast<FindResultsModelItem*>(this));
  }
  else {
    return 0;
  }
}
