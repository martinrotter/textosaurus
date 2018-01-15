// For license of this file, see <project-root-folder>/LICENSE.md.

#include "plugin-system/pluginfactory.h"

#include "gui/sidebars/findresultssidebar.h"
#include "gui/sidebars/outputsidebar.h"
#include "miscellaneous/textapplication.h"
#include "miscellaneous/textapplicationsettings.h"
#include "plugin-system/filesystem/filesystemplugin.h"
#include "plugin-system/markdown/markdownplugin.h"
#include "plugin-system/pluginbase.h"

#include <QAction>
#include <QMenu>

PluginFactory::PluginFactory(QObject* parent)
  : QObject(parent), m_plugins(QList<PluginBase*>()), m_sidebars(QList<BaseSidebar*>()),
  m_assignableActions(QList<QAction*>()), m_sidebarActions(QList<QAction*>()) {}

void PluginFactory::loadPlugins(TextApplication* text_app) {
  if (m_plugins.isEmpty()) {
    m_plugins << new MarkdownPlugin(this) << new FilesystemPlugin(this);

    for (PluginBase* plugin : m_plugins) {
      plugin->setTextApp(text_app);

      auto plugin_sidebars = plugin->sidebars();

      m_sidebars << plugin_sidebars;

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

void PluginFactory::hookPluginsIntoApplication(TextApplication* text_app) {
  text_app->m_menuDockWidgets->addActions(sidebarActions());
}
