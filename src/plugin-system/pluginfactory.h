// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef PLUGINFACTORY_H
#define PLUGINFACTORY_H

#include <QObject>

class PluginBase;
class DockWidget;
class TextApplication;

class PluginFactory : public QObject {
  Q_OBJECT

  public:
    explicit PluginFactory(QObject* parent = nullptr);

    QList<PluginBase*> plugins() const;
    QList<DockWidget*> sidebars() const;

  public slots:
    void loadPlugins(TextApplication* text_app);

  private:
    QList<PluginBase*> m_plugins;
    QList<DockWidget*> m_sidebars;
};

#endif // PLUGINFACTORY_H
