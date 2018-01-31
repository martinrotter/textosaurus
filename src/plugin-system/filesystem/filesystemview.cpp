#include "plugin-system/filesystem/filesystemview.h"

FilesystemView::FilesystemView(QWidget* parent) : QListView(parent) {}

void FilesystemView::setRootIndex(const QModelIndex& index) {
  QListView::setRootIndex(index);
  emit rootIndexChanged(index);
}
