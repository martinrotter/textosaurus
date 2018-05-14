// For license of this file, see <project-root-folder>/LICENSE.md.

#include "saurus/plugin-system/pluginfactory.h"

#include "saurus/gui/sidebars/findresultssidebar.h"
#include "saurus/gui/sidebars/outputsidebar.h"
#include "saurus/miscellaneous/application.h"
#include "saurus/miscellaneous/textapplication.h"
#include "saurus/miscellaneous/textapplicationsettings.h"
#include "saurus/plugin-system/filesystem/filesystemplugin.h"
#include "saurus/plugin-system/macros/macrosplugin.h"
#include "saurus/plugin-system/markdown/markdownplugin.h"
#include "saurus/plugin-system/pluginbase.h"

#include <QMenu>

PluginFactory::PluginFactory(QObject* parent)
  : QObject(parent), m_plugins(QList<PluginBase*>()), m_sidebars(QList<BaseSidebar*>()),
  m_assignableActions(QList<QAction*>()), m_sidebarActions(QList<QAction*>()) {}

void PluginFactory::loadPlugins(TextApplication* text_app) {
  if (m_plugins.isEmpty()) {
    // NOTE: If we decide to separate optional plugins
    // into separate assemblies, then we load them here
    // via QPluginLoader instead of these hardcoded references.
    m_plugins << new MarkdownPlugin(this) << new FilesystemPlugin(this) << new MacrosPlugin(this);

    for (PluginBase* plugin : m_plugins) {
      plugin->setTextApp(text_app, qApp->settings(), qApp->icons());

      auto plugin_sidebars = plugin->sidebars();

      m_sidebars << plugin_sidebars;
      m_assignableActions << plugin->userActions();

      for (BaseSidebar* sidebar : plugin_sidebars) {
        QAction* act_show = sidebar->generateAction();

        m_assignableActions << act_show;
        m_sidebarActions << act_show;
      }
    }
  }
}

QList<PluginBase*> PluginFactory::plugins() const {
  return m_plugins;
}

QList<BaseSidebar*> PluginFactory::sidebars() const {
  return m_sidebars;
}

QList<QAction*> PluginFactory::assignableActions() const {
  return m_assignableActions;
}

QList<QAction*> PluginFactory::sidebarActions() const {
  return m_sidebarActions;
}

QList<QAction*> PluginFactory::generateMenusForPlugins(QWidget* parent) const {
  QList<QAction*> menus;

  for (PluginBase* plugin : m_plugins) {
    auto plugin_actions = plugin->userActions();

    if (!plugin_actions.isEmpty()) {
      QMenu* menu_plugin = new QMenu(plugin->name(), parent);

      menu_plugin->addActions(plugin_actions);
      menus.append(menu_plugin->menuAction());
    }
  }

  return menus;
}

void PluginFactory::hookPluginsIntoApplication(TextApplication* text_app) {
  text_app->m_menuDockWidgets->addActions(sidebarActions());
}
