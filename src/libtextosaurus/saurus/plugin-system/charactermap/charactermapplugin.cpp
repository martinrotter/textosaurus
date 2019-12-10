// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "saurus/plugin-system/charactermap/charactermapplugin.h"

#include "definitions/definitions.h"

CharacterMapPlugin::CharacterMapPlugin()
{}

QString CharacterMapPlugin::name() const {
  return tr("Character Map");
}

QString CharacterMapPlugin::id() const {
  return QSL("io.github.martinrotter.textosaurus.charmap");
}

QList<BaseSidebar*> CharacterMapPlugin::sidebars() {
  return {};
}

QList<QAction*> CharacterMapPlugin::userActions() {
  return {};
}

void CharacterMapPlugin::start(QWidget* main_form_widget,
                               TextApplication* text_app,
                               Settings* settings,
                               IconFactory* icon_factory,
                               WebFactory* web_factory) {
  PluginBase::start(main_form_widget, text_app, settings, icon_factory, web_factory);
}

void CharacterMapPlugin::stop() {}
