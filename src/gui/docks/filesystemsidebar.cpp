// For license of this file, see <project-root-folder>/LICENSE.md.

#include "src/gui/docks/filesystemsidebar.h"

#include "gui/plaintoolbutton.h"
#include "miscellaneous/application.h"
#include "miscellaneous/iconfactory.h"

#include <QFileSystemModel>
#include <QGroupBox>
#include <QListWidget>
#include <QMimeData>
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

    m_fsModel = new FileSystemSidebarModel(widget);
    m_fsView = new QListView(widget);
    m_lvFavorites = new FavoritesListWidget(widget);

    layout->setMargin(0);
    widget->setLayout(layout);

    // Initialize toolbar.
    QHBoxLayout* layout_toolbar = new QHBoxLayout(widget);
    PlainToolButton* btn_parent = new PlainToolButton(widget);
    PlainToolButton* btn_add_favorites = new PlainToolButton(widget);

    connect(btn_parent, &PlainToolButton::clicked, this, &FilesystemSidebar::goToParentFolder);
    connect(btn_add_favorites, &PlainToolButton::clicked, this, &FilesystemSidebar::addFileToFavorites);

    btn_parent->setIcon(qApp->icons()->fromTheme(QSL("go-up")));
    btn_parent->setToolTip(tr("Go to parent folder"));
    btn_add_favorites->setIcon(qApp->icons()->fromTheme(QSL("folder-favorites")));
    btn_add_favorites->setToolTip(tr("Add selected file to favorites"));
    layout_toolbar->addWidget(btn_parent);
    layout_toolbar->addWidget(btn_add_favorites);
    layout_toolbar->addStretch();
    layout_toolbar->setMargin(0);

    // Initialize FS browser
    m_fsView->setDragDropMode(QAbstractItemView::DragDropMode::NoDragDrop);
    m_fsView->setIconSize(QSize(12, 12));
    m_fsView->setModel(m_fsModel);
    m_fsModel->setRootPath(QString());
    m_fsView->setRootIndex(m_fsModel->index(qApp->documentsFolder()));

    connect(m_fsView, &QListView::doubleClicked, this, &FilesystemSidebar::openFileFolder);

    // Initialize favorites.
    connect(m_lvFavorites, &QListWidget::doubleClicked, this, &FilesystemSidebar::openFavoriteFile);

    QStringList saved_files = qApp->settings()->value(GROUP(General),
                                                      objectName() + QSL("_files"),
                                                      QStringList()).toStringList();

    foreach (const QString& file, saved_files) {
      m_lvFavorites->loadFileItem(file);
    }

    layout->addLayout(layout_toolbar, 0);
    layout->addWidget(m_fsView, 1);
    layout->addWidget(m_lvFavorites, 1);

    setWidget(widget);
  }
}

void FilesystemSidebar::addFileToFavorites() {
  const QFileInfo file_info = m_fsModel->fileInfo(m_fsView->currentIndex());

  if (file_info.isFile()) {
    m_lvFavorites->loadFileItem(QDir::toNativeSeparators(file_info.absoluteFilePath()));
  }

  saveFavorites();
}

void FilesystemSidebar::openFavoriteFile(const QModelIndex& idx) {
  emit openFileRequested(m_lvFavorites->item(idx.row())->data(Qt::UserRole).toString());
}

void FilesystemSidebar::openFileFolder(const QModelIndex& idx) {
  if (m_fsModel->isDir(idx)) {
    m_fsView->setRootIndex(idx);
  }
  else {
    emit openFileRequested(m_fsModel->filePath(idx));
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

void FilesystemSidebar::saveFavorites() const {
  QStringList favorites;

  for (int i = 0; i < m_lvFavorites->count(); i++) {
    favorites.append(m_lvFavorites->item(i)->data(Qt::UserRole).toString());
  }

  qApp->settings()->setValue(GROUP(General),
                             objectName() + QSL("_files"),
                             favorites);
}

FileSystemSidebarModel::FileSystemSidebarModel(QObject* parent) : QFileSystemModel(parent) {}

FavoritesListWidget::FavoritesListWidget(QWidget* parent) : QListWidget(parent) {
  setDragDropMode(QAbstractItemView::DragDropMode::NoDragDrop);
}

void FavoritesListWidget::loadFileItem(const QString& file_path) {
  QListWidgetItem* item = new QListWidgetItem(this);

  item->setData(Qt::UserRole, file_path);
  item->setToolTip(file_path);

  if (!QFile::exists(file_path)) {
    item->setText(QFileInfo(file_path).fileName() + tr(" (N/A)"));
    item->setForeground(Qt::darkRed);
  }
  else {
    item->setText(QFileInfo(file_path).fileName());
  }
}

void FavoritesListWidget::keyPressEvent(QKeyEvent* event) {
  if (event->key() == Qt::Key_Delete) {
    event->accept();

    int row = currentRow();

    if (row >= 0) {
      delete takeItem(row);
    }
  }
  else {
    QListWidget::keyPressEvent(event);
  }
}
