// For license of this file, see <project-root-folder>/LICENSE.md.

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
    virtual void start(QWidget* main_form_widget, TextApplication* text_app, Settings* settings, IconFactory* icon_factory) override;
    virtual void stop() override;
    TextApplication* textApp() const;
    Settings* settings() const;
    IconFactory* iconFactory() const;
    FilesystemSidebar* sidebar() const;

  protected:
    TextApplication* m_textApp;
    Settings* m_settings;
    IconFactory* m_iconFactory;
    FilesystemSidebar* m_sidebar = nullptr;
};

#endif // FILESYSTEMPLUGIN_H
