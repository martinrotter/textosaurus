// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "saurus/plugin-system/pluginfactory.h"

#include "definitions/definitions.h"
#include "saurus/gui/sidebars/findresultssidebar.h"
#include "saurus/gui/sidebars/outputsidebar.h"
#include "saurus/miscellaneous/application.h"
#include "saurus/miscellaneous/textapplication.h"
#include "saurus/miscellaneous/textapplicationsettings.h"
#include "saurus/plugin-system/charactermap/charactermapplugin.h"
#include "saurus/plugin-system/clipboard/clipboardplugin.h"
#include "saurus/plugin-system/filesystem/filesystemplugin.h"
#include "saurus/plugin-system/macros/macrosplugin.h"
#include "saurus/plugin-system/markdown/markdownplugin.h"
#include "saurus/plugin-system/pluginbase.h"

#include <QDir>
#include <QMenu>
#include <QPluginLoader>

PluginFactory::PluginFactory(QObject* parent) : QObject(parent) {}

void PluginFactory::loadPlugins(TextApplication* text_app) {
  // Some hardcoded "plugins".
  m_plugins << new MarkdownPlugin(this) << new FilesystemPlugin(this)
            << new MacrosPlugin(this) << new CharacterMapPlugin(this)
            << new ClipboardPlugin(this);

  const QString plugins_path = pluginsLibPath();
  const auto backup_current_dir = QDir::currentPath();

  for (const QFileInfo& plugin_subdir : QDir(plugins_path).entryInfoList(QDir::Filter::Dirs)) {
    for (const QFileInfo& plugin_lib_file :
         QDir(plugin_subdir.absoluteFilePath()).entryInfoList({QSL("libtextosaurus-*") + pluginSuffix()})) {
      // Add plugin's base folder to library search path and
      // temporarily switch application's working directory.
      qApp->addLibraryPath(plugin_subdir.absoluteFilePath());
      QDir::setCurrent(plugin_subdir.absoluteFilePath());

      if (QLibrary::isLibrary(plugin_lib_file.absoluteFilePath())) {
        m_plugins << PluginState(plugin_lib_file.absoluteFilePath());
      }
    }
  }

  QDir::setCurrent(backup_current_dir);

  for (PluginState& plugin_state : m_plugins) {
    auto plugin = plugin_state.plugin();

    if (plugin == nullptr) {
      qCriticalNN << QSL("Cannot hook plugin '") << plugin_state.pluginLibraryFile() << QSL("' into application.");
      continue;
    }

    plugin->start(plugin_state.pluginLibraryFile(), qApp->mainFormWidget(),
                  text_app, qApp->settings(),
                  qApp->icons(), qApp->web());

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

QString PluginFactory::pluginSuffix() const {
#if defined(Q_OS_LINUX)
  return QSL(".so");
#elif defined(Q_OS_WIN)
  return QSL(".dll");
#elif defined(Q_OS_MACOS)
  return QSL(".dylib");
#else
  return QSL("");
#endif
}

QString PluginFactory::pluginsLibPath() const {
#if defined(Q_OS_LINUX)
  return qApp->applicationDirPath() + QDir::separator() +
          QL1S("..") + QDir::separator() +
          QL1S("lib") + QDir::separator() +
          QL1S(APP_LOW_NAME) + QDir::separator() +
          QL1S("plugins");
#else
  return qApp->applicationDirPath() + QDir::separator() + QL1S("plugins");
#endif
}

QList<PluginState> PluginFactory::plugins() const {
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

QList<QAction*> PluginFactory::generateMenusForPlugins(QWidget* parent) {
  QList<QAction*> menus;

  for (PluginState& plugin_state : m_plugins) {
    auto plugin = plugin_state.plugin();

    if (plugin == nullptr) {
      qCritical().noquote().nospace() << QSL("Cannot hook plugin '")
                                      << plugin_state.pluginLibraryFile()
                                      << QSL("' into application.");
      continue;
    }

    auto plugin_actions = plugin->userActions();

    if (!plugin_actions.isEmpty()) {
      QMenu* menu_plugin = new QMenu(plugin->name(), parent);

      menu_plugin->addActions(plugin_actions);
      menus.append(menu_plugin->menuAction());
    }
  }

  return menus;
}

void PluginFactory::quit() {
  for (PluginState& plugin_state : m_plugins) {
    auto plugin = plugin_state.plugin();

    if (plugin != nullptr) {
      plugin->stop();
    }
  }
}

void PluginFactory::addPlugin(PluginBase* plugin) {
  m_plugins.append(plugin);
}

PluginState::PluginState() : m_isLoaded(false), m_isBuiltin(false) {}

PluginState::PluginState(PluginBase* builtin_plugin) {
  // We load built-in plugin.
  m_isLoaded = true;
  m_isBuiltin = true;
  m_plugin = builtin_plugin;
  m_lastError = m_pluginAuthor = QString();
  m_pluginId = m_plugin->id();
  m_pluginName = m_plugin->name();
  m_pluginWebsite = APP_URL;
  m_pluginAuthor = QSL("Martin Rotter");
}

PluginState::PluginState(const QString& library_file) {
  // We have to load plugin from library.
  QPluginLoader loader(library_file);
  QObject* plugin_instance = loader.instance();

  m_pluginLibraryFile = library_file;
  m_plugin = qobject_cast<PluginBase*>(plugin_instance);
  m_pluginId = loader.metaData()["IID"].toString();
  m_pluginName = loader.metaData()["MetaData"].toObject()["name"].toString();
  m_pluginWebsite = loader.metaData()["MetaData"].toObject()["website"].toString();
  m_pluginAuthor = loader.metaData()["MetaData"].toObject()["author"].toString();
  m_isBuiltin = false;

  if (m_plugin != nullptr && !m_plugin->name().isEmpty() && !m_plugin->id().isEmpty()) {
    m_isLoaded = true;
    m_lastError = QString();

    qDebugNN << QSL("Successfully loaded plugin '") << m_pluginLibraryFile << QSL("%s'.");
  }
  else {
    m_plugin = nullptr;
    m_isLoaded = false;
    m_lastError = loader.errorString();

    loader.unload();

    qCriticalNN << QSL("Cannot load plugin '") << m_pluginLibraryFile << QSL("', error is: '") << m_lastError << QSL("'.");
  }
}

void PluginState::enable() {
  qApp->settings()->remove(GROUP(DisabledPlugins), pluginId());
}

void PluginState::disable() {
  qApp->settings()->setValue(GROUP(DisabledPlugins), pluginId(), true);
}

bool PluginState::isLoaded() const {
  return m_isLoaded;
}

bool PluginState::isBuiltin() const {
  return m_isBuiltin;
}

QString PluginState::lastError() const {
  return m_lastError;
}

PluginBase* PluginState::plugin() const {
  return m_plugin;
}

QString PluginState::pluginWebsite() const {
  return m_pluginWebsite;
}

QString PluginState::pluginAuthor() const {
  return m_pluginAuthor;
}

QString PluginState::pluginName() const {
  return m_pluginName;
}

QString PluginState::pluginLibraryFile() const {
  return m_pluginLibraryFile;
}

QString PluginState::pluginId() const {
  return m_pluginId;
}
