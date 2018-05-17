// For license of this file, see <project-root-folder>/LICENSE.md.

#include "saurus/plugin-system/filesystem/filesystemmodel.h"

#include "common/miscellaneous/iconfactory.h"
#include "saurus/plugin-system/filesystem/filesystemplugin.h"

FilesystemModel::FilesystemModel(FilesystemPlugin* plugin, QObject* parent) : QFileSystemModel(parent), m_plugin(plugin) {}

QVariant FilesystemModel::data(const QModelIndex& index, int role) const {
  if (role == Qt::ItemDataRole::DecorationRole) {
    if (isDir(index)) {
      return m_plugin->iconFactory()->fromTheme(QSL("folder"));
    }
    else {
      return m_plugin->iconFactory()->fromTheme(QSL("gtk-file"));
    }
  }
  else {
    return QFileSystemModel::data(index, role);
  }
}
