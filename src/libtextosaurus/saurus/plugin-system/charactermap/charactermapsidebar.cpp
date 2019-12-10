// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "saurus/plugin-system/charactermap/charactermapsidebar.h"

#include "saurus/plugin-system/charactermap/charactermapplugin.h"

CharacterMapSidebar::CharacterMapSidebar(CharacterMapPlugin* plugin, QWidget* parent)
  : BaseSidebar(plugin->textApp(), parent) {}

Qt::DockWidgetArea CharacterMapSidebar::initialArea() const {
  return Qt::DockWidgetArea::LeftDockWidgetArea;
}

bool CharacterMapSidebar::initiallyVisible() const {
  return true;
}

int CharacterMapSidebar::initialWidth() const {
  return 150;
}

void CharacterMapSidebar::load() {}
