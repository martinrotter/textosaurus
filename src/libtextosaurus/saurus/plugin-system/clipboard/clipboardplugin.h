// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifndef CLIPBOARDPLUGIN_H
#define CLIPBOARDPLUGIN_H

#include <QObject>

#include "saurus/plugin-system/pluginbase.h"

class ClipboardSidebar;

class ClipboardPlugin : public QObject, public PluginBase {
  Q_OBJECT
  Q_INTERFACES(PluginBase)

  public:
    explicit ClipboardPlugin(QObject* parent = nullptr);

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
    ClipboardSidebar* sidebar();

  private:
    ClipboardSidebar* m_sidebar;
};

#endif // CLIPBOARDPLUGIN_H
