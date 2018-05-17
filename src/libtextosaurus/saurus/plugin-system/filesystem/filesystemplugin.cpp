// For license of this file, see <project-root-folder>/LICENSE.md.

#include "saurus/plugin-system/filesystem/filesystemplugin.h"

#include "definitions/definitions.h"
#include "saurus/plugin-system/filesystem/filesystemsidebar.h"

FilesystemPlugin::FilesystemPlugin(QObject* parent) : QObject(parent) {}

QString FilesystemPlugin::name() const {
  return tr("Filesystem");
}

QList<BaseSidebar*> FilesystemPlugin::sidebars() {
  if (m_sidebar == nullptr) {
    m_sidebar = new FilesystemSidebar(this, nullptr);
  }

  return QList<BaseSidebar*>() << m_sidebar;
}

QList<QAction*> FilesystemPlugin::userActions() {
  return QList<QAction*>();
}

void FilesystemPlugin::start(TextApplication* text_app, Settings* settings, IconFactory* icon_factory) {
  m_textApp = text_app;
  m_settings = settings;
  m_iconFactory = icon_factory;
}

void FilesystemPlugin::stop() {}

TextApplication* FilesystemPlugin::textApp() const {
  return m_textApp;
}

Settings* FilesystemPlugin::settings() const {
  return m_settings;
}

IconFactory* FilesystemPlugin::iconFactory() const {
  return m_iconFactory;
}

FilesystemSidebar* FilesystemPlugin::sidebar() const
{
  return m_sidebar;
}
