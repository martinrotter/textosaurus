// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef FINDRESULTSMODEL_H
#define FINDRESULTSMODEL_H

#include <QAbstractItemModel>

class FindResultsModelItem;

class FindResultsModel : public QAbstractItemModel {
  Q_OBJECT

  public:
    explicit FindResultsModel(QObject* parent = nullptr);
    virtual ~FindResultsModel();

    QModelIndex index(int row, int column, const QModelIndex& parent) const;
    QModelIndex parent(const QModelIndex& child) const;
    int rowCount(const QModelIndex& parent) const;
    int columnCount(const QModelIndex& parent) const;
    QVariant data(const QModelIndex& index, int role) const;

  private:
    FindResultsModelItem* m_rootItem;
};

#endif // FINDRESULTSMODEL_H
