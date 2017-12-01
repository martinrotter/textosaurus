// For license of this file, see <project-root-folder>/LICENSE.md.

#include "src/gui/docks/filesystemsidebar.h"

FilesystemSidebar::FilesystemSidebar(QWidget* parent) : DockWidget(parent) {
  setWindowTitle(tr("Filesystem"));
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
