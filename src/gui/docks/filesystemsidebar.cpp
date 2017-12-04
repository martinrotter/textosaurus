// For license of this file, see <project-root-folder>/LICENSE.md.

#include "src/gui/docks/filesystemsidebar.h"

#include "miscellaneous/application.h"

#include <QFileSystemModel>
#include <QGroupBox>
#include <QListView>
#include <QVBoxLayout>

void FilesystemSidebar::initializeComponents() {
  QWidget* widget = new QWidget(this);
  QVBoxLayout* layout = new QVBoxLayout(widget);

  widget->setLayout(layout);

  QListView* fs_view = new QListView(widget);
  QListView* lv_favorites = new QListView(widget);

  // Initialize FS browser.
  QFileSystemModel* fs_model = new QFileSystemModel(fs_view);

  fs_view->setModel(fs_model);
  fs_view->setRootIndex(fs_model->setRootPath(qApp->documentsFolder()));

  connect(fs_view, &QListView::doubleClicked, this, [fs_view, fs_model](const QModelIndex& idx) {
    if (fs_model->isDir(idx)) {
      fs_view->setRootIndex(idx);
    }
    else {
      // TODO: otevřít soubor v editoru
    }
  });

  // Initialize favorites.
  layout->addWidget(fs_view, 1);
  layout->addWidget(lv_favorites, 1);

  setWidget(widget);
}

FilesystemSidebar::FilesystemSidebar(QWidget* parent) : DockWidget(parent) {
  setWindowTitle(tr("Filesystem"));
  initializeComponents();
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

void FilesystemSidebar::load() {}
