// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "saurus/plugin-system/clipboard/clipboardmodel.h"

#include "definitions/definitions.h"

#include <QApplication>
#include <QClipboard>
#include <QDir>
#include <QImage>
#include <QMimeData>
#include <QPixmap>
#include <QUrl>

ClipboardItem::ClipboardItem(QObject* parent)
  : QObject(parent), m_data(nullptr), m_time(QDateTime::currentDateTime()) {
  decideType();
}

ClipboardItem::ClipboardItem(QMimeData* data, QObject* parent)
  : QObject(parent), m_data(data), m_time(QDateTime::currentDateTime()) {
  decideType();
}

ClipboardItem::~ClipboardItem() {
  clearChildren();
}

QString ClipboardItem::mimeType() const {
  return m_data == nullptr ? QString() : m_data->formats().join(QL1S(", "));
}

QString ClipboardItem::heading(bool simple_view) const {
  switch (m_type) {
    case ClipboardItem::ItemType::File:
    case ClipboardItem::ItemType::Url: {
      auto urls = m_data->urls();
      QStringList str_urls;

      for (const QUrl& url :m_data->urls()) {
        str_urls << tr("%1").arg(url.isLocalFile()
                                 ? QDir::toNativeSeparators(url.toLocalFile())
                                 : url.toString());
      }

      return str_urls.join(QL1S("\n"));
    }

    case ClipboardItem::ItemType::Html:
      return simple_view
             ? (m_data->text().size() > 64 ? m_data->text().left(64).append(QSL("...")) : m_data->text().left(100))
                                           :  m_data->html();

    case ClipboardItem::ItemType::Text:
      return simple_view
             ? (m_data->text().size() > 64 ? m_data->text().left(64).append(QSL("...")) : m_data->text().left(100))
                                           :  m_data->text();

    case ClipboardItem::ItemType::Color:
      return qvariant_cast<QColor>(m_data->colorData()).name();

    default:
      return "-";
  }
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

ClipboardItem* ClipboardItem::child(int row) const {
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

void ClipboardItem::decideType() {
  if (m_data == nullptr) {
    // Root item.
    m_type = ItemType::None;
  }
  else if (m_data->hasUrls()) {
    auto urls = m_data->urls();

    for (const QUrl& url : m_data->urls()) {
      if (url.isLocalFile()) {
        // Return path to local file.
        m_type = ItemType::File;
        return;
      }
    }

    // Return general URL.
    m_type = ItemType::Url;
  }
  else if (m_data->hasHtml()) {
    // Return HTML.
    m_type = ItemType::Html;
  }
  else if (m_data->hasText()) {
    // Return plain text.
    m_type = ItemType::Text;
  }
  else if (m_data->hasImage()) {
    // Return picture data.
    m_type = ItemType::Picture;
  }
  else {
    // Return raw data for first MIME type.
    m_type = ItemType::Unknown;
  }
}

ClipboardItem::ItemType ClipboardItem::type() const {
  return m_type;
}

QMimeData* ClipboardItem::data() const {
  return m_data.data();
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
  : QAbstractItemModel(parent), m_rootItem(new ClipboardItem(this)), m_clipboard(qApp->clipboard()) {
  connect(m_clipboard, &QClipboard::changed, this, &ClipboardModel::processClipboardChange);
}

void ClipboardModel::processClipboardChange(QClipboard::Mode mode) {
  if (mode == QClipboard::Mode::Clipboard) {
    auto* dat = m_clipboard->mimeData(QClipboard::Mode::Clipboard);

    if (dat == nullptr || dat->formats().empty()) {
      return;
    }

    auto* ite = new QMimeData();

    for (auto form : dat->formats()) {
      ite->setData(form, QByteArray(dat->data(form)));
    }

    m_rootItem->prependChild(new ClipboardItem(ite, this));

    emit layoutChanged();
    emit clipboardChanged();
  }
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

  if (parentItem == m_rootItem.data()) {
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
  return 3;
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

          case 1: {
            switch (item->type()) {
              case ClipboardItem::ItemType::Url:
                return "URL";

              case ClipboardItem::ItemType::File:
                return "File";

              case ClipboardItem::ItemType::Html:
                return "HTML";

              case ClipboardItem::ItemType::Text:
                return "Text";

              case ClipboardItem::ItemType::Color:
                return "Color";

              case ClipboardItem::ItemType::Picture:
                return "Picture";

              case ClipboardItem::ItemType::None:
              case ClipboardItem::ItemType::Unknown:
                return "-";
            }
          }

          case 2:
            return item->heading(true);
        }
      }

      case Qt::ItemDataRole::ToolTipRole:
        return tr("<h2>MIME type</h2>%1"
                  "<h2>Contents</h2>%2").arg(item->mimeType(),
                                             item->heading(true).toHtmlEscaped().replace(QL1S("\n"), QL1S("<br/>")));

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
            return tr("Type");

          case 2:
            return tr("Contents");
        }
      }

      default:
        return QVariant();
    }
  }
}

ClipboardItem* ClipboardModel::newestEntry() {
  if (m_rootItem->childItems().isEmpty()) {
    return nullptr;
  }
  else {
    return m_rootItem->child(0);
  }
}

bool ClipboardModel::isEmpty() const {
  return m_rootItem->childCount() <= 0;
}

ClipboardItem* ClipboardModel::itemForIndex(const QModelIndex& idx) const {
  if (idx.isValid() && idx.model() == this) {
    return static_cast<ClipboardItem*>(idx.internalPointer());
  }
  else {
    return m_rootItem.data();
  }
}
