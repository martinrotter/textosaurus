// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef MARKDOWNPLUGIN_H
#define MARKDOWNPLUGIN_H

#include <QObject>

#include "plugin-system/pluginbase.h"

class MarkdownSidebar;

class MarkdownPlugin : public QObject, public PluginBase {
  Q_OBJECT

  //Q_PLUGIN_METADATA(IID "com.github.textosaurus.markdown")

  public:
    explicit MarkdownPlugin(QObject* parent = nullptr);

    virtual QString name() const override;
    virtual QList<BaseSidebar*> sidebars() override;
    virtual QList<QAction*> userActions() override;

  private:
    MarkdownSidebar* sidebar();

  private:
    MarkdownSidebar* m_sidebar;
};

#endif // MARKDOWNPLUGIN_H
