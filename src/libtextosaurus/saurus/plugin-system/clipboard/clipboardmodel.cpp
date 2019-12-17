// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "saurus/plugin-system/clipboard/clipboardmodel.h"

#include "definitions/definitions.h"

#include <QApplication>
#include <QClipboard>
#include <QMimeData>

ClipboardItem::ClipboardItem(QObject* parent)
  : QObject(parent), m_time(QDateTime::currentDateTime()) {}

ClipboardItem::ClipboardItem(QMimeData* data, QObject* parent)
  : QObject(parent), m_data(data), m_time(QDateTime::currentDateTime()) {}

ClipboardItem::~ClipboardItem() {
  if (m_data != nullptr) {
    delete m_data;
  }
}

QString ClipboardItem::mimeType() const {
  return m_data == nullptr ? QString() : m_data->formats().join(QL1C('\n'));
}

QString ClipboardItem::heading() const {
  return m_data == nullptr ? QString() : m_data->text();
}

void ClipboardItem::appendChild(ClipboardItem* item) {
  m_childItems.append(item);
  item->setParentItem(this);
  item->setParent(this);
}

void ClipboardItem::prependChild(ClipboardItem* item) {
  m_childItems.prepend(item);
  item->setParentItem(this);
  item->setParent(this);
}

ClipboardItem* ClipboardItem::child(int row) {
  return m_childItems.value(row);
}

int ClipboardItem::childCount() const {
  return m_childItems.count();
}

ClipboardItem* ClipboardItem::parentItem() const {
  return m_parentItem;
}

void ClipboardItem::setParentItem(ClipboardItem* parent_item) {
  m_parentItem = parent_item;
}

QList<ClipboardItem*> ClipboardItem::childItems() const {
  return m_childItems;
}

void ClipboardItem::clearChildren() {
  for (ClipboardItem* child : m_childItems) {
    child->deleteLater();
  }

  m_childItems.clear();
}

QDateTime ClipboardItem::time() const {
  return m_time;
}

int ClipboardItem::row() const {
  if (m_parentItem != nullptr) {
    return m_parentItem->m_childItems.indexOf(const_cast<ClipboardItem*>(this));
  }
  else {
    return 0;
  }
}

ClipboardModel::ClipboardModel(QObject* parent)
  : QAbstractItemModel(parent), m_rootItem(new ClipboardItem(this)), m_clipboard(qApp->clipboard()),
  m_lastDetection(QDateTime::currentDateTime().addSecs(-1)) {
  connect(m_clipboard, &QClipboard::changed, this, [=](QClipboard::Mode mode) {
    if (m_lastDetection.addMSecs(100) > QDateTime::currentDateTime()) {
      return;
    }

    if (mode == QClipboard::Mode::Clipboard) {
      auto* dat = m_clipboard->mimeData(QClipboard::Mode::Clipboard);

      if (dat == nullptr) {
        return;
      }

      auto* ite = new QMimeData();

      for (auto form : dat->formats()) {
        ite->setData(form, QByteArray(dat->data(form)));
      }

      m_lastDetection = QDateTime::currentDateTime();
      m_rootItem->prependChild(new ClipboardItem(ite, this));

      emit layoutChanged();
    }
  });
}

QModelIndex ClipboardModel::index(int row, int column, const QModelIndex& parent) const {
  if (!hasIndex(row, column, parent)) {
    return QModelIndex();
  }

  ClipboardItem* parentItem = itemForIndex(parent);
  ClipboardItem* childItem = parentItem->child(row);

  if (childItem != nullptr) {
    return createIndex(row, column, childItem);
  }
  else {
    return QModelIndex();
  }
}

QModelIndex ClipboardModel::parent(const QModelIndex& child) const {
  if (!child.isValid()) {
    return QModelIndex();
  }

  ClipboardItem* childItem = itemForIndex(child);
  ClipboardItem* parentItem = childItem->parentItem();

  if (parentItem == m_rootItem) {
    return QModelIndex();
  }
  else {
    return createIndex(parentItem->row(), 0, parentItem);
  }
}

int ClipboardModel::rowCount(const QModelIndex& parent) const {
  if (parent.column() > 0) {
    return 0;
  }

  auto* parentItem = itemForIndex(parent);

  return parentItem->childCount();
}

int ClipboardModel::columnCount(const QModelIndex& parent) const {
  Q_UNUSED(parent)
  return 2;
}

QVariant ClipboardModel::data(const QModelIndex& index, int role) const {
  if (!index.isValid()) {
    return QVariant();
  }
  else {
    auto* item = itemForIndex(index);

    if (item == nullptr) {
      return QVariant();
    }

    switch (role) {
      case Qt::ItemDataRole::DisplayRole: {
        switch (index.column()) {
          case 0:
            return item->time();

          case 1:
            return item->heading();
        }
      }

      default:
        return QVariant();
    }
  }
}

QVariant ClipboardModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if (orientation != Qt::Orientation::Horizontal) {
    return QVariant();
  }
  else {
    switch (role) {
      case Qt::ItemDataRole::DisplayRole: {
        switch (section) {
          case 0:
            return tr("Time");

          case 1:
            return tr("Contents");
        }
      }

      default:
        return QVariant();
    }
  }
}

ClipboardItem* ClipboardModel::itemForIndex(const QModelIndex& idx) const {
  if (idx.isValid() && idx.model() == this) {
    return static_cast<ClipboardItem*>(idx.internalPointer());
  }
  else {
    return m_rootItem;
  }
}
