// For license of this file, see <project-root-folder>/LICENSE.md.

#include "src/gui/docks/filesystemsidebar.h"

#include "gui/plaintoolbutton.h"
#include "miscellaneous/application.h"
#include "miscellaneous/iconfactory.h"

#include <QFileSystemModel>
#include <QGroupBox>
#include <QListView>
#include <QVBoxLayout>

FilesystemSidebar::FilesystemSidebar(QWidget* parent) : DockWidget(parent), m_fsModel(nullptr) {
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

void FilesystemSidebar::load() {
  if (m_fsModel == nullptr) {
    QWidget* widget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(widget);

    layout->setMargin(0);

    m_fsModel = new FileSystemSidebarModel(widget);
    widget->setLayout(layout);

    // Initialize toolbar.
    QHBoxLayout* layout_toolbar = new QHBoxLayout(widget);

    layout_toolbar->setMargin(0);
    PlainToolButton* btn_parent = new PlainToolButton(widget);

    btn_parent->setIcon(qApp->icons()->fromTheme(QSL("go-parent-folder")));
    layout_toolbar->addWidget(btn_parent);
    layout_toolbar->addStretch();

    // Initialize FS browser
    QListView* fs_view = new QListView(widget);

    fs_view->setIconSize(QSize(12, 12));
    fs_view->setModel(m_fsModel);
    m_fsModel->setRootPath(QString());
    fs_view->setRootIndex(m_fsModel->index(qApp->documentsFolder()));

    connect(btn_parent, &PlainToolButton::clicked, this, [fs_view, this] {
      QModelIndex prnt = fs_view->rootIndex().parent();

      if (prnt.isValid()) {
        fs_view->setRootIndex(prnt);
      }
      else {
        fs_view->setRootIndex(m_fsModel->index(QString()));
      }
    });
    connect(fs_view, &QListView::doubleClicked, this, [fs_view, this](const QModelIndex& idx) {
      if (m_fsModel->isDir(idx)) {
        fs_view->setRootIndex(idx);
      }
      else {
        emit openFileRequested(QDir::toNativeSeparators(m_fsModel->filePath(idx)));
      }
    });

    // Initialize favorites.
    QListView* lv_favorites = new QListView(widget);

    layout->addLayout(layout_toolbar, 0);
    layout->addWidget(fs_view, 1);
    layout->addWidget(lv_favorites, 1);

    setWidget(widget);
  }
}

FileSystemSidebarModel::FileSystemSidebarModel(QObject* parent) : QFileSystemModel(parent) {}
