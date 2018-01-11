// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef MARKDOWNPLUGIN_H
#define MARKDOWNPLUGIN_H

#include <QObject>

#include "plugin-system/pluginbase.h"

class MarkdownPlugin : public QObject, public PluginBase {
  Q_OBJECT
  Q_PLUGIN_METADATA(IID "com.github.textilosaurus.markdown")

  public:
    explicit MarkdownPlugin(QObject* parent = nullptr);

    virtual QString name() const override;
    virtual QList<DockWidget*> sidebars() const override;
};

#endif // MARKDOWNPLUGIN_H
