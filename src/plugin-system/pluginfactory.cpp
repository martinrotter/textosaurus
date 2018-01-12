// For license of this file, see <project-root-folder>/LICENSE.md.

#include "plugin-system/pluginfactory.h"

#include "plugin-system/filesystem/filesystemplugin.h"
#include "plugin-system/markdown/markdownplugin.h"
#include "plugin-system/pluginbase.h"

PluginFactory::PluginFactory(QObject* parent) : QObject(parent), m_plugins(QList<PluginBase*>()), m_sidebars(QList<DockWidget*>()) {}

void PluginFactory::loadPlugins(TextApplication* text_app) {
  if (m_plugins.isEmpty()) {
    m_plugins << new MarkdownPlugin(this) << new FilesystemPlugin(this);

    for (PluginBase* plugin : m_plugins) {
      plugin->setTextApp(text_app);
      m_sidebars << plugin->sidebars();
    }
  }
}

QList<PluginBase*> PluginFactory::plugins() const {
  return m_plugins;
}

QList<DockWidget*> PluginFactory::sidebars() const {
  return m_sidebars;
}
