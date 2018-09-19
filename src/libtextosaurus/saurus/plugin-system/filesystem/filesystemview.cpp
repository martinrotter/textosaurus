// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

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

QString FilesystemView::normalizePath(const QString& path) const {
  return path.isEmpty() ? path : QDir(QDir::cleanPath(path)).canonicalPath();
}

QString FilesystemView::currentFolder() const {
  return QDir::toNativeSeparators(m_model->filePath(rootIndex()));
}

QString FilesystemView::selectedFileFolder() const {
  return m_model->filePath(currentIndex());
}

void FilesystemView::cdUp() {
  QModelIndex prnt = rootIndex().parent();

  if (prnt.isValid()) {
    openFolder(prnt);
  }
  else {
    openFolder(QString());
  }
}

void FilesystemView::openFolder(const QModelIndex& idx) {
  openFolder(m_model->filePath(idx));
}

void FilesystemView::openFolder(const QString& path) {
  auto can_path = normalizePath(path);

  qDebug(R"(Opening folder "%s" (canonical), "%s" (non-canonical).)", qPrintable(can_path), qPrintable(path));
  setRootIndex(m_model->index(can_path));
}
