// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef FINDRESULTSSIDEBAR_H
#define FINDRESULTSSIDEBAR_H

#include "gui/sidebars/dockwidget.h"

class TextApplication;

class FindResultsSidebar : public DockWidget {
  public:
    explicit FindResultsSidebar(TextApplication* app, QWidget* parent = nullptr);

    Qt::DockWidgetArea initialArea() const;
    bool initiallyVisible() const;
    int initialWidth() const;

  public slots:
    void load();
};

#endif // FINDRESULTSSIDEBAR_H
