// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef PLUGINBASE_H
#define PLUGINBASE_H

#include <QObject>

#include "saurus/gui/sidebars/basesidebar.h"

class TextApplication;
class Settings;
class IconFactory;

class PluginBase {
  public:
    explicit PluginBase() = default;
    virtual ~PluginBase() = default;

    virtual QString name() const = 0;
    virtual QList<BaseSidebar*> sidebars() = 0;
    virtual QList<QAction*> userActions() = 0;
    virtual void setTextApp(TextApplication* text_app, Settings* settings, IconFactory* icon_factory) = 0;
};

#endif // PLUGINBASE_H
