// For license of this file, see <project-root-folder>/LICENSE.md.

#include "saurus/plugin-system/filesystem/filesystemmodel.h"

#include "common/miscellaneous/iconfactory.h"
#include "saurus/miscellaneous/application.h"

FilesystemModel::FilesystemModel(QObject* parent) : QFileSystemModel(parent) {}

QVariant FilesystemModel::data(const QModelIndex& index, int role) const {
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
