// For license of this file, see <project-root-folder>/LICENSE.md.

#include "saurus/plugin-system/filesystem/favoriteslistwidget.h"

#include "common/miscellaneous/iconfactory.h"
#include "saurus/plugin-system/filesystem/filesystemplugin.h"
#include "saurus/plugin-system/filesystem/filesystemsidebar.h"

#include <QFileInfo>
#include <QKeyEvent>

FavoritesListWidget::FavoritesListWidget(FilesystemPlugin* plugin, QWidget* parent) : QListWidget(parent), m_plugin(plugin) {
  setDragDropMode(QAbstractItemView::DragDropMode::NoDragDrop);
}

void FavoritesListWidget::loadFileItem(const QString& file_path) {
  QListWidgetItem* item = new QListWidgetItem(this);
  QFileInfo info(file_path);

  item->setData(Qt::UserRole, file_path);
  item->setToolTip(file_path);

  if (info.isDir()) {
    item->setIcon(m_plugin->iconFactory()->fromTheme(QSL("folder")));
  }
  else {
    item->setIcon(m_plugin->iconFactory()->fromTheme(QSL("gtk-file")));
  }

  if (!info.exists()) {
    item->setText(QFileInfo(file_path).fileName() + tr(" (N/A)"));
    item->setTextColor(Qt::darkRed);
  }
  else {
    item->setText(QFileInfo(file_path).fileName());
  }
}

void FavoritesListWidget::keyPressEvent(QKeyEvent* event) {
  if (event->key() == Qt::Key::Key_Delete) {
    event->accept();

    int row = currentRow();

    if (row >= 0) {
      delete takeItem(row);
      m_plugin->sidebar()->saveFavorites();
    }
  }
  else {
    QListWidget::keyPressEvent(event);
  }
}
