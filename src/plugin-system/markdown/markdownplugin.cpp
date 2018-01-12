// For license of this file, see <project-root-folder>/LICENSE.md.

#include "plugin-system/markdown/markdownplugin.h"

#include "definitions/definitions.h"
#include "plugin-system/markdown/markdownsidebar.h"

MarkdownPlugin::MarkdownPlugin(QObject* parent) : QObject(parent) {}

QString MarkdownPlugin::name() const {
  return QSL("Markdown");
}

QList<DockWidget*> MarkdownPlugin::sidebars() const {
  return QList<DockWidget*>() << new MarkdownSidebar(m_textApp, nullptr);
}
