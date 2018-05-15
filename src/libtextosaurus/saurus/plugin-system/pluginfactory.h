// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef PLUGINFACTORY_H
#define PLUGINFACTORY_H

#include <QObject>

class PluginBase;
class BaseSidebar;
class TextApplication;
class QAction;

class PluginFactory : public QObject {
  Q_OBJECT

  public:
    explicit PluginFactory(QObject* parent = nullptr);

    QList<PluginBase*> plugins() const;
    QList<BaseSidebar*> sidebars() const;
    QList<QAction*> assignableActions() const;
    QList<QAction*> sidebarActions() const;
    QList<QAction*> generateMenusForPlugins(QWidget* parent) const;

    void hookPluginsIntoApplication(TextApplication* text_app);

  public slots:
    void loadPlugins(TextApplication* text_app);

  private:
    QString pluginsLibPath() const;

  private:
    QList<PluginBase*> m_plugins;
    QList<BaseSidebar*> m_sidebars;
    QList<QAction*> m_assignableActions;
    QList<QAction*> m_sidebarActions;
};

#endif // PLUGINFACTORY_H
