// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "saurus/plugin-system/filesystem/filesystemplugin.h"

#include "definitions/definitions.h"
#include "saurus/plugin-system/filesystem/filesystemsidebar.h"

FilesystemPlugin::FilesystemPlugin(QObject* parent) : QObject(parent) {}

QString FilesystemPlugin::name() const {
  return tr("Filesystem");
}

QString FilesystemPlugin::id() const {
  return QSL("io.github.martinrotter.textosaurus.fs");
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

void FilesystemPlugin::start(const QString& library_file, QWidget* main_form_widget, TextApplication* text_app,
                             Settings* settings, IconFactory* icon_factory, WebFactory* web_factory) {
  PluginBase::start(library_file, main_form_widget, text_app, settings, icon_factory, web_factory);
}

void FilesystemPlugin::stop() {}

FilesystemSidebar* FilesystemPlugin::sidebar() const {
  return m_sidebar;
}
