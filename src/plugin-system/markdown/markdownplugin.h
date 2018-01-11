// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef MARKDOWNPLUGIN_H
#define MARKDOWNPLUGIN_H

#include <QObject>

#include "plugin-system/pluginbase.h"

class MarkdownPlugin : public QObject, public PluginBase {
  Q_OBJECT

  public:
    explicit MarkdownPlugin(QObject* parent = nullptr);

    virtual QString name() const override;
    virtual QList<DockWidget*> sidebars() const override;

  signals:

  public slots:
};

#endif // MARKDOWNPLUGIN_H
