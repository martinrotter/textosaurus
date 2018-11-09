// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "saurus/plugin-system/filesystem/filesystemview.h"

#include "definitions/definitions.h"
#include "saurus/plugin-system/filesystem/filesystemmodel.h"

#include <QDebug>
#include <QKeyEvent>
#include <QMenu>
#include <QtGlobal>

#if defined(Q_OS_WIN)
#include <Windows.h>
#endif

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

  qDebug().noquote().nospace() << QSL("Opening folder '")
                               << can_path
                               << QSL("' (canonical), '")
                               << path
                               << QSL("' (non-canonical).");
  setRootIndex(m_model->index(can_path));
}

void FilesystemView::highlightFile(const QString& file_path) {
  QFileInfo info_file(file_path);

  openFolder(info_file.absolutePath());
  setCurrentIndex(m_model->index(file_path));
}

void FilesystemView::contextMenuEvent(QContextMenuEvent* event) {
#if defined(Q_OS_WIN)
  QString file_under_menu = m_model->filePath(indexAt(event->pos()));

  if (!file_under_menu.isEmpty()) {
    QMenu menu(tr("FS Sidebar Context Menu"), this);

    menu.addAction(tr("Properties"), [file_under_menu]() {
      SHELLEXECUTEINFO info = {0};
      auto xx = file_under_menu;
      auto cc = xx.toStdWString();
      auto aa = cc.c_str();

      info.lpFile = aa;
      info.nShow = SW_SHOW;
      info.fMask = SEE_MASK_INVOKEIDLIST;
      info.lpVerb = L"properties";
      info.cbSize = sizeof info;

      ShellExecuteEx(&info);
    });
    menu.exec(event->globalPos());
    event->accept();
  }
  else {
    event->ignore();
  }
#else
  QListView::contextMenuEvent(event);
#endif
}
