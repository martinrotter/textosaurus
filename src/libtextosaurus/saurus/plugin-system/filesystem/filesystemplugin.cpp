// For license of this file, see <project-root-folder>/LICENSE.md.

#include "saurus/plugin-system/filesystem/filesystemplugin.h"

#include "definitions/definitions.h"
#include "saurus/plugin-system/filesystem/filesystemsidebar.h"

FilesystemPlugin::FilesystemPlugin(QObject* parent) : QObject(parent) {}

QString FilesystemPlugin::name() const {
  return QSL("Filesystem");
}

QList<BaseSidebar*> FilesystemPlugin::sidebars() {
  return QList<BaseSidebar*>() << new FilesystemSidebar(m_textApp, nullptr);
}

QList<QAction*> FilesystemPlugin::userActions() {
  return QList<QAction*>();
}

void FilesystemPlugin::setTextApp(TextApplication* text_app) {
  m_textApp = text_app;
}
