// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef PLUGINFACTORY_H
#define PLUGINFACTORY_H

#include <QObject>

class PluginBase;

class PluginFactory : public QObject {
  Q_OBJECT

  public:
    explicit PluginFactory(QObject* parent = nullptr);

    QList<PluginBase*> plugins() const;

  public slots:
    void loadPlugins();

  private:
    QList<PluginBase*> m_plugins;
};

#endif // PLUGINFACTORY_H
