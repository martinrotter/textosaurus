// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "saurus/plugin-system/markdown/markdownplugin.h"

#include "definitions/definitions.h"
#include "saurus/miscellaneous/textapplication.h"
#include "saurus/plugin-system/markdown/markdownsidebar.h"

#include <QAction>

MarkdownPlugin::MarkdownPlugin(QObject* parent)
  : QObject(parent), m_sidebar(nullptr) {}

QString MarkdownPlugin::name() const {
  return QSL("Markdown Preview");
}

QString MarkdownPlugin::id() const {
  return QSL("io.github.martinrotter.textosaurus.markdownsimple");
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
    m_sidebar = new MarkdownSidebar(this, nullptr);
  }

  return m_sidebar;
}

void MarkdownPlugin::start(const QString& library_file, QWidget* main_form_widget, TextApplication* text_app,
                           Settings* settings, IconFactory* icon_factory, WebFactory* web_factory) {
  PluginBase::start(library_file, main_form_widget, text_app, settings, icon_factory, web_factory);
}

void MarkdownPlugin::stop() {}
