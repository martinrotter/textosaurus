// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifndef PLUGINFACTORY_H
#define PLUGINFACTORY_H

#include <QObject>

class PluginBase;
class BaseSidebar;
class TextApplication;
class QAction;

class TEXTOSAURUS_DLLSPEC PluginState {

  public:
    PluginState();
    PluginState(PluginBase* builtin_plugin);
    explicit PluginState(const QString& library_file);

    void enable();
    void disable();

    // Determines whether this plugin will be loaded on next boot or not.
    bool isEnabled() const;

    bool isLoaded() const;
    bool isBuiltin() const;

    QString lastError() const;
    QString pluginId() const;
    QString pluginName() const;
    QString pluginWebsite() const;
    QString pluginAuthor() const;
    QString pluginLibraryFile() const;
    PluginBase* plugin() const;

  private:
    bool m_isLoaded;
    bool m_isBuiltin;
    QString m_lastError;
    QString m_pluginId;
    QString m_pluginName;
    QString m_pluginWebsite;
    QString m_pluginAuthor;
    QString m_pluginLibraryFile;
    PluginBase* m_plugin = nullptr;
};

Q_DECLARE_METATYPE(PluginState)

class TEXTOSAURUS_DLLSPEC PluginFactory : public QObject {
  Q_OBJECT

  public:
    explicit PluginFactory(QObject* parent = nullptr);

    QList<PluginState> plugins() const;
    QList<BaseSidebar*> sidebars() const;
    QList<QAction*> assignableActions() const;
    QList<QAction*> sidebarActions() const;
    QList<QAction*> generateMenusForPlugins(QWidget* parent);

  public slots:
    void quit();

    // Adds the instance of plugin to list of plugins:
    // WARNING: This needs to be called before loadPlugins(...)
    // gets called.
    void addPlugin(PluginBase* plugin);
    void loadPlugins(TextApplication* text_app);

  private:
    QString pluginSuffix() const;
    QString pluginsLibPath() const;

  private:
    QList<PluginState> m_plugins;
    QList<BaseSidebar*> m_sidebars;
    QList<QAction*> m_assignableActions;
    QList<QAction*> m_sidebarActions;
};

#endif // PLUGINFACTORY_H
