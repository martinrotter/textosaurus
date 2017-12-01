// For license of this file, see <project-root-folder>/LICENSE.md.

#include "src/gui/docks/filesystemsidebar.h"

FilesystemSidebar::FilesystemSidebar(QWidget* parent) : DockWidget(parent) {
  setWindowTitle(tr("Filesystem"));
  setContentsMargins(0, 0, 0, 0);
  setFeatures(QDockWidget::DockWidgetFeature::DockWidgetClosable | QDockWidget::DockWidgetFeature::DockWidgetMovable);
  setAllowedAreas(Qt::DockWidgetArea::LeftDockWidgetArea |
                  Qt::DockWidgetArea::RightDockWidgetArea |
                  Qt::DockWidgetArea::BottomDockWidgetArea);
}

Qt::DockWidgetArea FilesystemSidebar::initialArea() const {
  return Qt::DockWidgetArea::LeftDockWidgetArea;
}

bool FilesystemSidebar::initiallyVisible() const {
  return true;
}

int FilesystemSidebar::initialWidth() const {
  return 150;
}
