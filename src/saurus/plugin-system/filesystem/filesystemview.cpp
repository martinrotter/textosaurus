// For license of this file, see <project-root-folder>/LICENSE.md.

#include "saurus/plugin-system/filesystem/filesystemview.h"

#include "saurus/plugin-system/filesystem/filesystemmodel.h"

#include <QKeyEvent>

FilesystemView::FilesystemView(FilesystemModel* model, QWidget* parent) : QListView(parent), m_model(model) {}

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

FilesystemModel* FilesystemView::model() const {
  return m_model;
}

void FilesystemView::cdUp() {
  QModelIndex prnt = rootIndex().parent();

  setRootIndex(prnt.isValid() ? prnt : m_model->index(m_model->rootPath()));
}
