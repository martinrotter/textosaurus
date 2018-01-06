// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef FINDRESULTSMODEL_H
#define FINDRESULTSMODEL_H

#include <QAbstractItemModel>

class FindResultsModelItem;
class FindResultsModelItemEditor;
class TextEditor;

class FindResultsModel : public QAbstractItemModel {
  Q_OBJECT

  public:
    explicit FindResultsModel(QObject* parent = nullptr);
    virtual ~FindResultsModel();

    virtual QModelIndex index(int row, int column, const QModelIndex& parent) const override;
    virtual QModelIndex parent(const QModelIndex& child) const override;
    virtual int rowCount(const QModelIndex& parent) const override;
    virtual int columnCount(const QModelIndex& parent) const override;
    virtual QVariant data(const QModelIndex& index, int role) const override;

    void addResults(TextEditor* editor, const QList<QPair<int, int>> results);
    FindResultsModelItem* itemForIndex(const QModelIndex& idx) const;

  private:
    FindResultsModelItem* m_rootItem;
};

#endif // FINDRESULTSMODEL_H
