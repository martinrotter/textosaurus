// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef FINDRESULTSSIDEBAR_H
#define FINDRESULTSSIDEBAR_H

#include "gui/sidebars/dockwidget.h"

#include <QAbstractListModel>

#include "gui/texteditor.h"

class TextApplication;
class QTreeWidget;

class FindResultsSidebar : public DockWidget {
  Q_OBJECT

  public:
    explicit FindResultsSidebar(TextApplication* app, QWidget* parent = nullptr);

    Qt::DockWidgetArea initialArea() const;
    bool initiallyVisible() const;
    int initialWidth() const;

  public slots:
    void load();
    void addResults(TextEditor* editor, const QList<QPair<int, int>> results);

  private:
    QTreeWidget* m_viewResults;
};

#endif // FINDRESULTSSIDEBAR_H
