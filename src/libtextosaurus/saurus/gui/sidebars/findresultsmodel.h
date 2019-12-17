// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifndef FINDRESULTSMODEL_H
#define FINDRESULTSMODEL_H

#include <QAbstractItemModel>
#include <QStyledItemDelegate>

#include "saurus/gui/sidebars/findresultsmodelitem.h"
#include "saurus/gui/sidebars/findresultsmodelitemeditor.h"
#include "saurus/gui/sidebars/findresultsmodelitemresult.h"

class TextEditor;

class FindResultsModel : public QAbstractItemModel {
  Q_OBJECT

  public:
    explicit FindResultsModel(QObject* parent = nullptr);

    virtual QModelIndex index(int row, int column, const QModelIndex& parent) const override;
    virtual QModelIndex parent(const QModelIndex& child) const override;
    virtual int rowCount(const QModelIndex& parent) const override;
    virtual int columnCount(const QModelIndex& parent) const override;
    virtual QVariant data(const QModelIndex& index, int role) const override;
    FindResultsModelItem* itemForIndex(const QModelIndex& idx) const;

  public slots:
    void clear();
    void addResults(TextEditor* editor, const QList<QPair<int, int>>& results);

  private:
    QScopedPointer<FindResultsModelItem> m_rootItem;
};

class HtmlDelegate : public QStyledItemDelegate {
  Q_OBJECT

  public:
    explicit HtmlDelegate(QObject* parent = nullptr);
    virtual ~HtmlDelegate() = default;

  protected:
    void paint ( QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index ) const;
    QSize sizeHint ( const QStyleOptionViewItem& option, const QModelIndex& index ) const;
};

#endif // FINDRESULTSMODEL_H
