// For license of this file, see <project-root-folder>/LICENSE.md.

#include "plugin-system/markdown/markdownplugin.h"

#include "definitions/definitions.h"
#include "miscellaneous/application.h"
#include "miscellaneous/iconfactory.h"
#include "plugin-system/markdown/markdownsidebar.h"

#include <QAction>

MarkdownPlugin::MarkdownPlugin(QObject* parent) : QObject(parent), m_sidebar(nullptr) {}

QString MarkdownPlugin::name() const {
  return QSL("Markdown");
}

QList<BaseSidebar*> MarkdownPlugin::sidebars() {
  return QList<BaseSidebar*>() << sidebar();
}

QList<QAction*> MarkdownPlugin::userActions() {
  return QList<QAction*>({
    sidebar()->m_actionRefreshPreview
  });
}

MarkdownSidebar* MarkdownPlugin::sidebar() {
  if (m_sidebar == nullptr) {
    m_sidebar = new MarkdownSidebar(m_textApp, nullptr);
  }

  return m_sidebar;
}
