// For license of this file, see <project-root-folder>/LICENSE.md.

#include "saurus/plugin-system/macros/macrosplugin.h"

#include "common/miscellaneous/settings.h"
#include "saurus/miscellaneous/application.h"
#include "saurus/plugin-system/macros/macros.h"
#include "saurus/plugin-system/macros/macrossidebar.h"

MacrosPlugin::MacrosPlugin(QObject* parent)
  : QObject(parent), m_macrosFactory(new Macros(qApp->settings(), this)), m_sidebar(nullptr) {}

QString MacrosPlugin::name() const {
  return tr("Macros");
}

QList<BaseSidebar*> MacrosPlugin::sidebars() {
  return QList<BaseSidebar*>() << sidebar();
}

QList<QAction*> MacrosPlugin::userActions() {
  return {};
}

void MacrosPlugin::setTextApp(TextApplication* text_app, Settings* settings, IconFactory* icon_factory) {
  m_textApp = text_app;
}

MacrosSidebar* MacrosPlugin::sidebar() {
  if (m_sidebar == nullptr) {
    m_sidebar = new MacrosSidebar(m_textApp, m_macrosFactory, nullptr);
  }

  return m_sidebar;
}
