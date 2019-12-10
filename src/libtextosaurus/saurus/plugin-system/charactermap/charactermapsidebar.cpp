// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "saurus/plugin-system/charactermap/charactermapsidebar.h"

#include "definitions/definitions.h"
#include "saurus/gui/tabwidget.h"
#include "saurus/gui/texteditor.h"
#include "saurus/miscellaneous/textapplication.h"
#include "saurus/plugin-system/charactermap/charactermap.h"
#include "saurus/plugin-system/charactermap/charactermapplugin.h"
#include "saurus/plugin-system/charactermap/charactermapwidget.h"

CharacterMapSidebar::CharacterMapSidebar(CharacterMapPlugin* plugin, QWidget* parent)
  : BaseSidebar(plugin->textApp(), parent), m_mapWidget(nullptr) {
  setWindowTitle(tr("Character Map"));
  setObjectName(QSL("m_sidebarCharacterMap"));
}

Qt::DockWidgetArea CharacterMapSidebar::initialArea() const {
  return Qt::DockWidgetArea::LeftDockWidgetArea;
}

bool CharacterMapSidebar::initiallyVisible() const {
  return true;
}

int CharacterMapSidebar::initialWidth() const {
  return 150;
}

void CharacterMapSidebar::load() {
  if (m_mapWidget == nullptr) {
    m_mapWidget = new CharacterMapWidget(this);

    connect(m_mapWidget->map(), &CharacterMap::characterSelected, this, &CharacterMapSidebar::onCharactedSelected);

    setWidget(m_mapWidget);
    BaseSidebar::load();
  }
}

void CharacterMapSidebar::onCharactedSelected(const QString& chr) {
  TextEditor* editor = m_textApp->tabWidget()->currentEditor();

  if (editor != nullptr) {
    QByteArray output_utf = chr.toUtf8();

    editor->insertText(editor->currentPos(), output_utf.constData());
    editor->gotoPos(editor->currentPos() + output_utf.size());
  }
}
