// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifndef FILESYSTEMPLUGIN_H
#define FILESYSTEMPLUGIN_H

#include <QObject>

#include "saurus/plugin-system/pluginbase.h"

class FilesystemSidebar;

class FilesystemPlugin : public QObject, public PluginBase {
  Q_OBJECT
  Q_INTERFACES(PluginBase)

  public:
    explicit FilesystemPlugin(QObject* parent = nullptr);

    virtual QString name() const override;
    virtual QString id() const override;
    virtual QList<BaseSidebar*> sidebars() override;
    virtual QList<QAction*> userActions() override;
    virtual void start(const QString& library_file, QWidget* main_form_widget,
                       TextApplication* text_app, Settings* settings,
                       IconFactory* icon_factory, WebFactory* web_factory) override;
    virtual void stop() override;
    FilesystemSidebar* sidebar() const;

  private:
    FilesystemSidebar* m_sidebar = nullptr;
};

#endif // FILESYSTEMPLUGIN_H
