// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifndef MARKDOWNPLUGIN_H
#define MARKDOWNPLUGIN_H

#include <QObject>

#include "saurus/plugin-system/pluginbase.h"

class MarkdownSidebar;

class MarkdownPlugin : public QObject, public PluginBase {
  Q_OBJECT

  //Q_PLUGIN_METADATA(IID "io.github.martinrotter.textosaurus.markdownsimple")
  Q_INTERFACES(PluginBase)

  public:
    explicit MarkdownPlugin(QObject* parent = nullptr);

    virtual QString name() const override;
    virtual QString id() const override;
    virtual QList<BaseSidebar*> sidebars() override;
    virtual QList<QAction*> userActions() override;
    virtual void start(QWidget* main_form_widget, TextApplication* text_app, Settings* settings, IconFactory* icon_factory) override;
    virtual void stop() override;

  protected:
    TextApplication* m_textApp;

  private:
    MarkdownSidebar* sidebar();

  private:
    MarkdownSidebar* m_sidebar;
};

#endif // MARKDOWNPLUGIN_H
