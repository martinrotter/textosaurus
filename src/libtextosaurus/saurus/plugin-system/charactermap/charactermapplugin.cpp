// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "saurus/plugin-system/charactermap/charactermapplugin.h"

#include "definitions/definitions.h"
#include "saurus/plugin-system/charactermap/charactermapsidebar.h"

CharacterMapPlugin::CharacterMapPlugin(QObject* parent) : QObject(parent), m_sidebar(nullptr)
{}

QString CharacterMapPlugin::name() const {
  return tr("Character Map");
}

QString CharacterMapPlugin::id() const {
  return QSL("io.github.martinrotter.textosaurus.charmap");
}

QList<BaseSidebar*> CharacterMapPlugin::sidebars() {
  if (m_sidebar == nullptr) {
    m_sidebar = new CharacterMapSidebar(this, nullptr);
  }

  return QList<BaseSidebar*>() << m_sidebar;
}

QList<QAction*> CharacterMapPlugin::userActions() {
  return {};
}

void CharacterMapPlugin::start(const QString& library_file, QWidget* main_form_widget,
                               TextApplication* text_app, Settings* settings,
                               IconFactory* icon_factory, WebFactory* web_factory) {
  PluginBase::start(library_file, main_form_widget, text_app, settings, icon_factory, web_factory);
}

void CharacterMapPlugin::stop() {}
