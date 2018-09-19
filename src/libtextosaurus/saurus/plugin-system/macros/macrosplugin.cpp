// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "saurus/plugin-system/macros/macrosplugin.h"

#include "common/miscellaneous/settings.h"
#include "saurus/miscellaneous/application.h"
#include "saurus/plugin-system/macros/macros.h"
#include "saurus/plugin-system/macros/macrossidebar.h"

MacrosPlugin::MacrosPlugin(QObject* parent)
  : QObject(parent), m_textApp(nullptr), m_iconFactory(nullptr), m_settings(nullptr), m_macrosFactory(nullptr), m_sidebar(nullptr) {}

QString MacrosPlugin::name() const {
  return tr("Macros");
}

QString MacrosPlugin::id() const {
  return QSL("io.github.martinrotter.textosaurus.macros");
}

QList<BaseSidebar*> MacrosPlugin::sidebars() {
  return QList<BaseSidebar*>() << sidebar();
}

QList<QAction*> MacrosPlugin::userActions() {
  return {};
}

void MacrosPlugin::start(QWidget* main_form_widget, TextApplication* text_app,
                         Settings* settings, IconFactory* icon_factory, WebFactory* web_factory) {
  Q_UNUSED(web_factory)

  m_mainForm = main_form_widget;
  m_textApp = text_app;
  m_iconFactory = icon_factory;
  m_settings = settings;
  m_macrosFactory = new Macros(this, this);
}

void MacrosPlugin::stop() {}

MacrosSidebar* MacrosPlugin::sidebar() {
  if (m_sidebar == nullptr) {
    m_sidebar = new MacrosSidebar(this, m_macrosFactory, nullptr);
  }

  return m_sidebar;
}

Settings* MacrosPlugin::settings() const {
  return m_settings;
}

QWidget* MacrosPlugin::mainForm() const {
  return m_mainForm;
}

TextApplication* MacrosPlugin::textApp() const {
  return m_textApp;
}

IconFactory* MacrosPlugin::iconFactory() const {
  return m_iconFactory;
}
