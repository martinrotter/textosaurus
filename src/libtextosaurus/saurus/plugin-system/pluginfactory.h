// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef PLUGINFACTORY_H
#define PLUGINFACTORY_H

#include <QObject>

class PluginBase;
class BaseSidebar;
class TextApplication;
class QAction;

class PluginState {

  public:
    PluginState(PluginBase* builtin_plugin);
    explicit PluginState(const QString& library_file);

    bool isLoaded() const;
    bool isRemovable() const;
    QString lastError() const;
    QString pluginId() const;
    QString pluginName() const;
    QString pluginAuthor() const;
    QString pluginLibraryFile() const;
    PluginBase* plugin() const;

  private:
    bool m_isLoaded;
    bool m_isRemovable;
    QString m_lastError;
    QString m_pluginId;
    QString m_pluginName;
    QString m_pluginAuthor;
    QString m_pluginLibraryFile;
    PluginBase* m_plugin;
};

class PluginFactory : public QObject {
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
    void loadPlugins(TextApplication* text_app);

  private:
    QString pluginsLibPath() const;

  private:
    QList<PluginState> m_plugins;
    QList<BaseSidebar*> m_sidebars;
    QList<QAction*> m_assignableActions;
    QList<QAction*> m_sidebarActions;
};

#endif // PLUGINFACTORY_H
