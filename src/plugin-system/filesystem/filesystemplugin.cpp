// For license of this file, see <project-root-folder>/LICENSE.md.

#include "plugin-system/filesystem/filesystemplugin.h"

#include "definitions/definitions.h"
#include "plugin-system/filesystem/filesystemsidebar.h"

FilesystemPlugin::FilesystemPlugin(QObject* parent) : QObject(parent) {}

QString FilesystemPlugin::name() const {
  return QSL("Filesystem");
}

QList<BaseSidebar*> FilesystemPlugin::sidebars() const {
  return QList<BaseSidebar*>() << new FilesystemSidebar(m_textApp, nullptr);
}
