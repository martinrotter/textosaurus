// For license of this file, see <project-root-folder>/LICENSE.md.

#include "plugin-system/filesystem/filesystemsidebarmodel.h"

#include "miscellaneous/application.h"
#include "miscellaneous/iconfactory.h"

FilesystemSidebarModel::FilesystemSidebarModel(QObject* parent) : QFileSystemModel(parent) {}

QVariant FilesystemSidebarModel::data(const QModelIndex& index, int role) const {
  if (role == Qt::ItemDataRole::DecorationRole) {
    return qApp->icons()->fromTheme(isDir(index) ? QSL("folder") : QSL("gtk-file"));
  }
  else {
    return QFileSystemModel::data(index, role);
  }
}
