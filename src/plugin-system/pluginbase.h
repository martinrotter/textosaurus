// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef PLUGINBASE_H
#define PLUGINBASE_H

#include <QObject>

#include "gui/sidebars/basesidebar.h"

class TextApplication;

class PluginBase {
  public:
    explicit PluginBase() = default;
    virtual ~PluginBase() = default;

    virtual QString name() const = 0;
    virtual QList<BaseSidebar*> sidebars() const = 0;

    void setTextApp(TextApplication* text_app);

  protected:
    TextApplication* m_textApp;
};

inline void PluginBase::setTextApp(TextApplication* text_app) {
  m_textApp = text_app;
}

#endif // PLUGINBASE_H
