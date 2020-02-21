// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifndef CLIPBOARDMODEL_H
#define CLIPBOARDMODEL_H

#include <QAbstractItemModel>
#include <QClipboard>
#include <QDateTime>

class ClipboardItem : public QObject {
  Q_OBJECT

  public:
    enum class ItemType {
      None,
      Unknown,
      Text,
      File,
      Url,
      Color,
      Html,
      Picture
    };

    explicit ClipboardItem(QObject* parent = nullptr);
    explicit ClipboardItem(QMimeData* data, QObject* parent = nullptr);
    virtual ~ClipboardItem();

    ItemType type() const;
    QString mimeType() const;
    QString heading(bool simple_view) const;
    QDateTime time() const;
    QMimeData* data() const;

    void appendChild(ClipboardItem* child);
    void prependChild(ClipboardItem* item);
    ClipboardItem* child(int row) const;

    int childCount() const;
    int row() const;

    ClipboardItem* parentItem() const;

    void setParentItem(ClipboardItem* parent_item);

    QList<ClipboardItem*> childItems() const;
    void clearChildren();

  private:
    void decideType();

  private:
    ItemType m_type;

    QScopedPointer<QMimeData> m_data;
    QDateTime m_time;

    QList<ClipboardItem*> m_childItems;
    ClipboardItem* m_parentItem;
};

class ClipboardModel : public QAbstractItemModel {
  Q_OBJECT

  public:
    explicit ClipboardModel(QObject* parent = nullptr);

    virtual QModelIndex index(int row, int column, const QModelIndex& parent) const override;
    virtual QModelIndex parent(const QModelIndex& child) const override;
    virtual int rowCount(const QModelIndex& parent) const override;
    virtual int columnCount(const QModelIndex& parent) const override;
    virtual QVariant data(const QModelIndex& index, int role) const override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    ClipboardItem* newestEntry();

    bool isEmpty() const;
    ClipboardItem* itemForIndex(const QModelIndex& idx) const;

  public slots:
    void processClipboardChange(QClipboard::Mode mode = QClipboard::Mode::Clipboard);

  signals:
    void clipboardChanged();

  private:
    QScopedPointer<ClipboardItem> m_rootItem;
    QClipboard* m_clipboard;
};

#endif // CLIPBOARDMODEL_H
