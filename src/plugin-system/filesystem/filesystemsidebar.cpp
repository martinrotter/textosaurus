// For license of this file, see <project-root-folder>/LICENSE.md.

#include "src/plugin-system/filesystem/filesystemsidebar.h"

#include "gui/plaintoolbutton.h"
#include "miscellaneous/application.h"
#include "miscellaneous/iconfactory.h"
#include "miscellaneous/syntaxhighlighting.h"
#include "miscellaneous/textapplication.h"

#include <QFileSystemModel>
#include <QGroupBox>
#include <QListWidget>
#include <QMimeData>
#include <QToolBar>
#include <QVBoxLayout>

FilesystemSidebar::FilesystemSidebar(TextApplication* text_app, QWidget* parent) : BaseSidebar(text_app, parent), m_fsModel(nullptr) {
  setWindowTitle(tr("Filesystem"));
  setObjectName(QSL("m_sidebarFilesystem"));

  connect(this, &FilesystemSidebar::openFileRequested, this, [this](const QString& file_path) {
    m_textApp->loadTextEditorFromFile(file_path);
  });
}

Qt::DockWidgetArea FilesystemSidebar::initialArea() const {
  return Qt::DockWidgetArea::LeftDockWidgetArea;
}

bool FilesystemSidebar::initiallyVisible() const {
  return true;
}

int FilesystemSidebar::initialWidth() const {
  return 200;
}

void FilesystemSidebar::load() {
  if (m_fsModel == nullptr) {
    QWidget* widget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(widget);

    m_fsModel = new FileSystemSidebarModel(widget);
    m_fsView = new FilesystemView(widget);
    m_lvFavorites = new FavoritesListWidget(widget);

    layout->setMargin(0);

    // Initialize toolbar.
    QToolBar* tool_bar = new QToolBar(widget);
    QAction* btn_parent = new QAction(qApp->icons()->fromTheme(QSL("go-up")),
                                      tr("Go to parent folder"), widget);
    QAction* btn_add_favorites = new QAction(qApp->icons()->fromTheme(QSL("folder-favorites")),
                                             tr("Add selected item to favorites"), widget);

    connect(btn_parent, &QAction::triggered, this, &FilesystemSidebar::goToParentFolder);
    connect(btn_add_favorites, &QAction::triggered, this, &FilesystemSidebar::addToFavorites);

    tool_bar->addAction(btn_parent);
    tool_bar->addAction(btn_add_favorites);
    tool_bar->setIconSize(QSize(16, 16));

    // Initialize FS browser
    m_fsModel->setNameFilterDisables(false);
    m_fsModel->setFilter(QDir::Filter::Dirs | QDir::Filter::Files | QDir::Filter::Hidden |
                         QDir::Filter::System | QDir::Filter::AllDirs | QDir::Filter::NoDotAndDotDot);
    m_fsModel->setNameFilters(m_textApp->settings()->syntaxHighlighting()->bareFileFilters());
    m_fsView->setDragDropMode(QAbstractItemView::DragDropMode::NoDragDrop);
    m_fsView->setIconSize(QSize(12, 12));
    m_fsView->setModel(m_fsModel);
    m_lvFavorites->setIconSize(QSize(12, 12));
    m_fsModel->setRootPath(QString());
    m_fsView->setRootIndex(m_fsModel->index(qApp->settings()->value(windowTitle().toLower(),
                                                                    QL1S("current_folder_") + OS_ID_LOW,
                                                                    qApp->documentsFolder()).toString()));

    connect(m_fsView, &QListView::doubleClicked, this, &FilesystemSidebar::openFileFolder);
    connect(m_fsView, &FilesystemView::rootIndexChanged, this, &FilesystemSidebar::saveCurrentFolder);

    // Initialize favorites.
    connect(m_lvFavorites, &QListWidget::doubleClicked, this, &FilesystemSidebar::openFavoriteItem);

    QStringList saved_files = qApp->settings()->value(windowTitle().toLower(),
                                                      QSL("favorites"),
                                                      QStringList()).toStringList();

    foreach (const QString& file, saved_files) {
      m_lvFavorites->loadFileItem(file);
    }

    m_lvFavorites->sortItems(Qt::SortOrder::AscendingOrder);

    layout->addWidget(tool_bar, 0);
    layout->addWidget(m_fsView, 1);
    layout->addWidget(m_lvFavorites, 1);

    setWidget(widget);
  }
}

void FilesystemSidebar::saveCurrentFolder(const QModelIndex& idx) {
  qApp->settings()->setValue(windowTitle().toLower(),
                             QL1S("current_folder_") + OS_ID_LOW,
                             QDir::toNativeSeparators((m_fsModel->filePath(idx))));
}

void FilesystemSidebar::addToFavorites() {
  const QFileInfo file_info = m_fsModel->fileInfo(m_fsView->currentIndex());

  if (file_info.isFile() || file_info.isDir()) {
    m_lvFavorites->loadFileItem(QDir::toNativeSeparators(file_info.absoluteFilePath()));
  }

  m_lvFavorites->sortItems(Qt::SortOrder::AscendingOrder);
  saveFavorites();
}

void FilesystemSidebar::openFavoriteItem(const QModelIndex& idx) {
  const auto file_folder = QFileInfo(m_lvFavorites->item(idx.row())->data(Qt::UserRole).toString());

  if (file_folder.isDir()) {
    m_fsView->setRootIndex(m_fsModel->index(file_folder.absoluteFilePath()));
  }
  else {
    emit openFileRequested(file_folder.absoluteFilePath());
  }
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

  qApp->settings()->setValue(windowTitle().toLower(), QSL("favorites"), favorites);
}

FileSystemSidebarModel::FileSystemSidebarModel(QObject* parent) : QFileSystemModel(parent) {}

FavoritesListWidget::FavoritesListWidget(QWidget* parent) : QListWidget(parent) {
  setDragDropMode(QAbstractItemView::DragDropMode::NoDragDrop);
}

void FavoritesListWidget::loadFileItem(const QString& file_path) {
  QListWidgetItem* item = new QListWidgetItem(this);
  QFileInfo info(file_path);

  item->setData(Qt::UserRole, file_path);
  item->setToolTip(file_path);
  item->setIcon(qApp->icons()->fromTheme(info.isDir() ? QSL("folder") : QSL("gtk-file")));

  if (!info.exists()) {
    item->setText(QFileInfo(file_path).fileName() + tr(" (N/A)"));
    item->setTextColor(Qt::darkRed);
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

FilesystemView::FilesystemView(QWidget* parent) : QListView(parent) {}

void FilesystemView::setRootIndex(const QModelIndex& index) {
  QListView::setRootIndex(index);
  emit rootIndexChanged(index);
}
