// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifndef FINDRESULTSSIDEBAR_H
#define FINDRESULTSSIDEBAR_H

#include "saurus/gui/sidebars/basesidebar.h"

#include <QAbstractListModel>

#include "saurus/gui/texteditor.h"

class TextApplication;
class FindResultsModel;
class QTreeView;

class FindResultsSidebar : public BaseSidebar {
  Q_OBJECT

  public:
    explicit FindResultsSidebar(TextApplication* app, QWidget* parent = nullptr);

    virtual Qt::DockWidgetArea initialArea() const override;
    virtual bool initiallyVisible() const override;
    virtual int initialWidth() const override;

  public slots:
    void addResults(TextEditor* editor, const QList<QPair<int, int>>& results);

  private slots:
    void navigateToResult(const QModelIndex& index);

  private:
    void load() override;

  private:
    QTreeView* m_viewResults;
    FindResultsModel* m_model;
};

#endif // FINDRESULTSSIDEBAR_H
