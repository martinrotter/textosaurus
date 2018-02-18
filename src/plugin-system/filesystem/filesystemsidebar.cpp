// For license of this file, see <project-root-folder>/LICENSE.md.

#include "src/plugin-system/filesystem/filesystemsidebar.h"

#include "definitions/definitions.h"
#include "gui/baselineedit.h"
#include "gui/plaintoolbutton.h"
#include "miscellaneous/application.h"
#include "miscellaneous/iconfactory.h"
#include "miscellaneous/syntaxhighlighting.h"
#include "miscellaneous/textapplication.h"
#include "plugin-system/filesystem/favoriteslistwidget.h"
#include "plugin-system/filesystem/filesystemmodel.h"
#include "plugin-system/filesystem/filesystemview.h"

#include <QComboBox>
#include <QFileSystemModel>
#include <QGroupBox>
#include <QListWidget>
#include <QMimeData>
#include <QStorageInfo>
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

void FilesystemSidebar::openDrive(int index) {
  QModelIndex idx = m_fsModel->index(index, 0, QModelIndex());

  m_fsView->setRootIndex(idx);
}

void FilesystemSidebar::load() {
  if (m_fsModel == nullptr) {
    m_tabWidget = new QTabWidget(this);
    QWidget* widget_browser = new QWidget(this);
    QVBoxLayout* layout_browser = new QVBoxLayout(widget_browser);

    m_fsModel = new FilesystemModel(widget_browser);
    m_fsView = new FilesystemView(m_fsModel, widget_browser);
    m_lvFavorites = new FavoritesListWidget(m_tabWidget);
    m_txtPath = new BaseLineEdit(widget_browser);
    m_txtPath->setReadOnly(true);

    m_tabWidget->setTabPosition(QTabWidget::TabPosition::South);
    layout_browser->setMargin(0);

    // Initialize toolbar.
    QToolBar* tool_bar = new QToolBar(widget_browser);
    QAction* btn_parent = new QAction(qApp->icons()->fromTheme(QSL("go-up")),
                                      tr("Go to parent folder"), widget_browser);
    QAction* btn_add_favorites = new QAction(qApp->icons()->fromTheme(QSL("folder-favorites")),
                                             tr("Add selected item to favorites"), widget_browser);

    connect(btn_parent, &QAction::triggered, m_fsView, &FilesystemView::cdUp);
    connect(btn_add_favorites, &QAction::triggered, this, &FilesystemSidebar::addToFavorites);

    tool_bar->setFixedHeight(26);
    tool_bar->addAction(btn_parent);
    tool_bar->addAction(btn_add_favorites);
    tool_bar->setIconSize(QSize(16, 16));

    // Initialize FS browser.
    m_cmbDrives = new QComboBox(widget_browser);
    m_cmbDrives->setModel(m_fsModel);
    connect(m_cmbDrives, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &FilesystemSidebar::openDrive);

    m_fsModel->setNameFilterDisables(false);
    m_fsModel->setFilter(QDir::Filter::Files | QDir::Filter::Hidden | QDir::Filter::System | QDir::Filter::AllDirs | QDir::Filter::NoDot);
    m_fsModel->setNameFilters(m_textApp->settings()->syntaxHighlighting()->bareFileFilters());
    m_fsView->setDragDropMode(QAbstractItemView::DragDropMode::NoDragDrop);
    m_fsView->setIconSize(QSize(12, 12));
    m_fsView->setModel(m_fsModel);
    m_lvFavorites->setIconSize(QSize(12, 12));
    m_lvFavorites->setFrameShape(QFrame::Shape::NoFrame);
    m_fsView->setFrameShape(QFrame::Shape::NoFrame);
    m_fsModel->setRootPath(QString());
    m_fsView->setRootIndex(m_fsModel->index(qApp->settings()->value(m_settingsSection,
                                                                    QL1S("current_folder_") + OS_ID_LOW,
                                                                    qApp->documentsFolder()).toString()));
    saveCurrentFolder(m_fsView->rootIndex());

    connect(m_fsView, &QListView::activated, this, &FilesystemSidebar::openFileFolder);
    connect(m_fsView, &FilesystemView::rootIndexChanged, this, &FilesystemSidebar::saveCurrentFolder);
    connect(m_lvFavorites, &QListWidget::activated, this, &FilesystemSidebar::openFavoriteItem);
    connect(m_fsView, &FilesystemView::rootIndexChanged, this, [this]() {
      m_fsView->setFocus();
    });

    QStringList saved_files = qApp->settings()->value(m_settingsSection, QSL("favorites"), QStringList()).toStringList();

    foreach (const QString& file, saved_files) {
      m_lvFavorites->loadFileItem(file);
    }

    m_lvFavorites->sortItems(Qt::SortOrder::AscendingOrder);

    layout_browser->addWidget(tool_bar, 0);
    layout_browser->addWidget(m_cmbDrives, 0);
    layout_browser->addWidget(m_txtPath, 0);
    layout_browser->addWidget(m_fsView, 1);

    m_tabWidget->addTab(widget_browser, tr("Explorer"));
    m_tabWidget->addTab(m_lvFavorites, tr("Favorites"));

    setWidget(m_tabWidget);
    setFocusProxy(m_fsView);
  }
}

void FilesystemSidebar::saveCurrentFolder(const QModelIndex& idx) {
  auto path = QDir::toNativeSeparators((m_fsModel->fileInfo(idx).canonicalFilePath()));
  QStorageInfo file_drive(path);
  auto idx_root = m_fsModel->index(file_drive.rootPath()).row();

  m_cmbDrives->setCurrentIndex(idx_root);
  m_txtPath->setText(path);
  m_txtPath->setToolTip(path);
  qApp->settings()->setValue(m_settingsSection, QL1S("current_folder_") + OS_ID_LOW, path);
}

void FilesystemSidebar::addToFavorites() {
  const QFileInfo file_info = m_fsModel->fileInfo(m_fsView->currentIndex());

  if (file_info.isFile() || file_info.isDir()) {
    m_lvFavorites->loadFileItem(QDir::toNativeSeparators(file_info.absoluteFilePath()));
  }

  m_lvFavorites->sortItems(Qt::SortOrder::AscendingOrder);
  saveFavorites();
  makeFavoritesVisible();
}

void FilesystemSidebar::openFavoriteItem(const QModelIndex& idx) {
  const auto file_folder = QFileInfo(m_lvFavorites->item(idx.row())->data(Qt::UserRole).toString());

  if (file_folder.isDir()) {
    m_fsView->setRootIndex(m_fsModel->index(file_folder.absoluteFilePath()));
    makeExplorerVisible();
  }
  else {
    emit openFileRequested(file_folder.absoluteFilePath());
  }
}

void FilesystemSidebar::openFileFolder(const QModelIndex& idx) {
  if (m_fsModel->isDir(idx)) {
    // NOTE: This goes from index -> path -> index to properly
    // resolve ".." parent item.
    m_fsView->setRootIndex(m_fsModel->index(m_fsModel->filePath(idx)));
  }
  else {
    emit openFileRequested(m_fsModel->filePath(idx));
  }
}

void FilesystemSidebar::goToParentFolder() {
  QModelIndex prnt = m_fsView->rootIndex().parent();

  m_fsView->setRootIndex(prnt.isValid() ? prnt : m_fsModel->index(m_fsModel->rootPath()));
}

void FilesystemSidebar::saveFavorites() const {
  QStringList favorites;

  for (int i = 0; i < m_lvFavorites->count(); i++) {
    favorites.append(m_lvFavorites->item(i)->data(Qt::UserRole).toString());
  }

  qApp->settings()->setValue(m_settingsSection, QSL("favorites"), favorites);
}

void FilesystemSidebar::makeExplorerVisible() const {
  m_tabWidget->setCurrentIndex(0);
}

void FilesystemSidebar::makeFavoritesVisible() const {
  m_tabWidget->setCurrentIndex(1);
}
