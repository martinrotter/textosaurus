// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef MACROSPLUGIN_H
#define MACROSPLUGIN_H

#include <QObject>

#include "saurus/plugin-system/pluginbase.h"

class Macros;
class MacrosSidebar;

class MacrosPlugin : public QObject, public PluginBase {
  Q_OBJECT

  //Q_PLUGIN_METADATA(IID "io.github.martinrotter.textosaurus.markdown")

  public:
    explicit MacrosPlugin(QObject* parent = nullptr);

    virtual QString name() const override;
    virtual QList<BaseSidebar*> sidebars() override;
    virtual QList<QAction*> userActions() override;

  private:
    MacrosSidebar* sidebar();

  private:
    Macros* m_macrosFactory;
    MacrosSidebar* m_sidebar;
};

#endif // MACROSPLUGIN_H
