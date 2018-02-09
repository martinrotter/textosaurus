// For license of this file, see <project-root-folder>/LICENSE.md.

#include "plugin-system/filesystem/filesystemsidebarmodel.h"

#include "miscellaneous/application.h"
#include "miscellaneous/iconfactory.h"

FilesystemSidebarModel::FilesystemSidebarModel(QObject* parent) : QFileSystemModel(parent) {}

QVariant FilesystemSidebarModel::data(const QModelIndex& index, int role) const {
  if (role == Qt::ItemDataRole::DecorationRole) {
    if (isDir(index)) {
      return qApp->icons()->fromTheme(QSL("folder"));
    }
    else {
      return qApp->icons()->fromTheme(QSL("gtk-file"));
    }
  }
  else {
    return QFileSystemModel::data(index, role);
  }
}
