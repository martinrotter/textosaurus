// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef FILESYSTEMPLUGIN_H
#define FILESYSTEMPLUGIN_H

#include <QObject>

#include "saurus/plugin-system/pluginbase.h"

class FilesystemPlugin : public QObject, public PluginBase {
  Q_OBJECT

  //Q_PLUGIN_METADATA(IID "io.github.martinrotter.textosaurus.filesystem")
  Q_INTERFACES(PluginBase)

  public:
    explicit FilesystemPlugin(QObject* parent = nullptr);

    virtual QString name() const override;
    virtual QList<BaseSidebar*> sidebars() override;
    virtual QList<QAction*> userActions() override;
    virtual void start(TextApplication* text_app, Settings* settings, IconFactory* icon_factory) override;
    virtual void stop() override;

  protected:
    TextApplication* m_textApp;
};

#endif // FILESYSTEMPLUGIN_H
