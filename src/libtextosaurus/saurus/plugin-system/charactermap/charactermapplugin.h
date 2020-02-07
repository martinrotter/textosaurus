// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifndef CHARACTERMAPPLUGIN_H
#define CHARACTERMAPPLUGIN_H

#include <QObject>

#include "saurus/plugin-system/pluginbase.h"

class CharacterMapSidebar;

class CharacterMapPlugin : public QObject, public PluginBase {
  Q_OBJECT
  Q_INTERFACES(PluginBase)

  public:
    explicit CharacterMapPlugin(QObject* parent = nullptr);

    QString name() const override;
    QString id() const override;
    QList<BaseSidebar*> sidebars() override;
    QList<QAction*> userActions() override;
    void start(const QString& library_file,
               QWidget* main_form_widget,
               TextApplication* text_app,
               Settings* settings,
               IconFactory* icon_factory,
               WebFactory* web_factory) override;
    void stop() override;

  private:
    CharacterMapSidebar* m_sidebar;
};

#endif // CHARACTERMAPPLUGIN_H
