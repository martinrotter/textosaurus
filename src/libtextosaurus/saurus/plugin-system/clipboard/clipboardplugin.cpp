// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "saurus/plugin-system/clipboard/clipboardplugin.h"

#include "definitions/definitions.h"
#include "saurus/plugin-system/clipboard/clipboardsidebar.h"

ClipboardPlugin::ClipboardPlugin(QObject* parent) : QObject(parent), m_sidebar(nullptr) {}

QString ClipboardPlugin::name() const {
  return tr("Clipboard History");
}

QString ClipboardPlugin::id() const {
  return QSL("io.github.martinrotter.textosaurus.clipboard");
}

QList<BaseSidebar*> ClipboardPlugin::sidebars() {
  return { sidebar() };
}

QList<QAction*> ClipboardPlugin::userActions() {
  return { sidebar()->m_actionInsertLatestText };
}

void ClipboardPlugin::start(const QString& library_file, QWidget* main_form_widget,
                            TextApplication* text_app, Settings* settings,
                            IconFactory* icon_factory, WebFactory* web_factory) {
  PluginBase::start(library_file, main_form_widget, text_app, settings, icon_factory, web_factory);
}

void ClipboardPlugin::stop() {}

ClipboardSidebar* ClipboardPlugin::sidebar() {
  if (m_sidebar == nullptr) {
    m_sidebar = new ClipboardSidebar(this, nullptr);
  }

  return m_sidebar;
}
