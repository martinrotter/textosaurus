// For license of this file, see <project-root-folder>/LICENSE.md.

#include "gui/sidebars/findresultssidebar.h"

FindResultsSidebar::FindResultsSidebar(TextApplication* app, QWidget* parent) : DockWidget(parent) {}

Qt::DockWidgetArea FindResultsSidebar::initialArea() const {
  return Qt::DockWidgetArea::BottomDockWidgetArea;
}

bool FindResultsSidebar::initiallyVisible() const {
  return false;
}

int FindResultsSidebar::initialWidth() const {
  return 100;
}

void FindResultsSidebar::load() {}
