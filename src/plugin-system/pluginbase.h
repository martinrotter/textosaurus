// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef PLUGINBASE_H
#define PLUGINBASE_H

#include <QObject>

#include "gui/sidebars/dockwidget.h"

class PluginBase {
  public:
    explicit PluginBase() = default;
    virtual ~PluginBase() = default;

    virtual QString name() const = 0;
    virtual QList<DockWidget*> sidebars() const = 0;
};

#endif // PLUGINBASE_H
