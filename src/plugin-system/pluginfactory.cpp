// For license of this file, see <project-root-folder>/LICENSE.md.

#include "plugin-system/pluginfactory.h"

#include "plugin-system/markdown/markdownplugin.h"
#include "plugin-system/pluginbase.h"

PluginFactory::PluginFactory(QObject* parent) : QObject(parent), m_plugins(QList<PluginBase*>()) {}

void PluginFactory::loadPlugins() {
  if (m_plugins.isEmpty()) {
    // NOTE: Once we use plugins separated in shared libraries, then
    // here plugins will be loaded via QPluginLoader instances.
    m_plugins.append(new MarkdownPlugin(this));
  }
}

QList<PluginBase*> PluginFactory::plugins() const {
  return m_plugins;
}
