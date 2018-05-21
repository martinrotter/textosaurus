// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifndef MACROSPLUGIN_H
#define MACROSPLUGIN_H

#include <QObject>

#include "saurus/plugin-system/pluginbase.h"

class Macros;
class MacrosSidebar;

class MacrosPlugin : public QObject, public PluginBase {
  Q_OBJECT

  //Q_PLUGIN_METADATA(IID "io.github.martinrotter.textosaurus.macros")
  Q_INTERFACES(PluginBase)

  public:
    explicit MacrosPlugin(QObject* parent = nullptr);

    virtual QString name() const override;
    virtual QString id() const override;
    virtual QList<BaseSidebar*> sidebars() override;
    virtual QList<QAction*> userActions() override;
    virtual void start(QWidget* main_form_widget, TextApplication* text_app, Settings* settings, IconFactory* icon_factory) override;
    virtual void stop() override;
    IconFactory* iconFactory() const;
    TextApplication* textApp() const;
    QWidget* mainForm() const;
    Settings* settings() const;

  private:
    MacrosSidebar* sidebar();

  private:
    QWidget* m_mainForm;
    TextApplication* m_textApp;
    IconFactory* m_iconFactory;
    Settings* m_settings;
    Macros* m_macrosFactory;
    MacrosSidebar* m_sidebar;
};

#endif // MACROSPLUGIN_H
