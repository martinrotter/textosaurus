// For license of this file, see <project-root-folder>/LICENSE.md.

#include "plugin-system/filesystem/favoriteslistwidget.h"

#include "miscellaneous/application.h"
#include "miscellaneous/iconfactory.h"

#include <QFileInfo>

FavoritesListWidget::FavoritesListWidget(QWidget* parent) : QListWidget(parent) {
  setDragDropMode(QAbstractItemView::DragDropMode::NoDragDrop);
}

void FavoritesListWidget::loadFileItem(const QString& file_path) {
  QListWidgetItem* item = new QListWidgetItem(this);
  QFileInfo info(file_path);

  item->setData(Qt::UserRole, file_path);
  item->setToolTip(file_path);

  if (info.isDir()) {
    item->setIcon(qApp->icons()->fromTheme(QSL("folder")));
  }
  else {
    item->setIcon(qApp->icons()->fromTheme(QSL("gtk-file")));
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
    }
  }
  else {
    QListWidget::keyPressEvent(event);
  }
}
