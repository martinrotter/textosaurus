// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef FINDRESULTSMODELITEM_H
#define FINDRESULTSMODELITEM_H

#include <QObject>

class FindResultsModelItem : public QObject {
  Q_OBJECT

  public:
    explicit FindResultsModelItem(QObject* parent = nullptr);
    virtual ~FindResultsModelItem();

    virtual QVariant data(int role) const;

    void appendChild(FindResultsModelItem* child);
    FindResultsModelItem* child(int row);

    int childCount() const;
    int row() const;
    FindResultsModelItem* parentItem();

    void setParentItem(FindResultsModelItem* parent_item);

  private:
    QList<FindResultsModelItem*> m_childItems;
    FindResultsModelItem* m_parentItem;
};

#endif // FINDRESULTSMODELITEM_H
