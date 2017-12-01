#ifndef FILESYSTEMSIDEBAR_H
#define FILESYSTEMSIDEBAR_H

#include "src/gui/docks/dockwidget.h"

class FilesystemSidebar : public DockWidget {
  Q_OBJECT

  public:
    explicit FilesystemSidebar(QWidget* parent = nullptr);

    Qt::DockWidgetArea initialArea() const;
    bool initiallyVisible() const;
    int initialWidth() const;
};

#endif // FILESYSTEMSIDEBAR_H
