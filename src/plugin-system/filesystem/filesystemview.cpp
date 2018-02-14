// For license of this file, see <project-root-folder>/LICENSE.md.

#include "plugin-system/filesystem/filesystemview.h"

#include "plugin-system/filesystem/filesystemsidebarmodel.h"

#include <QKeyEvent>

FilesystemView::FilesystemView(QWidget* parent) : QListView(parent) {}

void FilesystemView::setRootIndex(const QModelIndex& index) {
  QListView::setRootIndex(index);
  emit rootIndexChanged(index);
}

void FilesystemView::keyPressEvent(QKeyEvent* event) {
  if (event->key() == Qt::Key::Key_Backspace) {
    cdUp();
  }
  else {
    QListView::keyPressEvent(event);
  }
}

FilesystemSidebarModel* FilesystemView::model() const {
  return m_model;
}

void FilesystemView::setModel(FilesystemSidebarModel* model) {
  m_model = model;
  QListView::setModel(model);
}

void FilesystemView::cdUp() {
  QModelIndex prnt = rootIndex().parent();

  setRootIndex(prnt.isValid() ? prnt : m_model->index(m_model->rootPath()));
}
