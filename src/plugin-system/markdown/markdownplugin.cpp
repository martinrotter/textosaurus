// For license of this file, see <project-root-folder>/LICENSE.md.

#include "plugin-system/markdown/markdownplugin.h"

#include "definitions/definitions.h"
#include "plugin-system/markdown/markdownsidebar.h"

MarkdownPlugin::MarkdownPlugin(QObject* parent) : QObject(parent) {}

QString MarkdownPlugin::name() const {
  return QSL("Markdown");
}

QList<BaseSidebar*> MarkdownPlugin::sidebars() const {
  return QList<BaseSidebar*>() << new MarkdownSidebar(m_textApp, nullptr);
}
