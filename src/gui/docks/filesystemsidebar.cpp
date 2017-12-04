// For license of this file, see <project-root-folder>/LICENSE.md.

#include "src/gui/docks/filesystemsidebar.h"

#include "gui/plaintoolbutton.h"
#include "miscellaneous/application.h"
#include "miscellaneous/iconfactory.h"

#include <QFileSystemModel>
#include <QGroupBox>
#include <QListWidget>
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
    m_fsView = new QListView(widget);

    m_fsView->setIconSize(QSize(12, 12));
    m_fsView->setModel(m_fsModel);
    m_fsModel->setRootPath(QString());
    m_fsView->setRootIndex(m_fsModel->index(qApp->documentsFolder()));

    connect(btn_parent, &PlainToolButton::clicked, this, &FilesystemSidebar::goToParentFolder);
    connect(m_fsView, &QListView::doubleClicked, this, &FilesystemSidebar::openFileFolder);

    // Initialize favorites.
    m_lvFavorites = new QListWidget(widget);

    layout->addLayout(layout_toolbar, 0);
    layout->addWidget(m_fsView, 1);
    layout->addWidget(m_lvFavorites, 1);

    setWidget(widget);
  }
}

void FilesystemSidebar::openFileFolder(const QModelIndex& idx) {
  if (m_fsModel->isDir(idx)) {
    m_fsView->setRootIndex(idx);
  }
  else {
    emit openFileRequested(QDir::toNativeSeparators(m_fsModel->filePath(idx)));
  }
}

void FilesystemSidebar::goToParentFolder() {
  QModelIndex prnt = m_fsView->rootIndex().parent();

  if (prnt.isValid()) {
    m_fsView->setRootIndex(prnt);
  }
  else {
    m_fsView->setRootIndex(m_fsModel->index(QString()));
  }
}

FileSystemSidebarModel::FileSystemSidebarModel(QObject* parent) : QFileSystemModel(parent) {}
