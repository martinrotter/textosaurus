// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "saurus/plugin-system/clipboard/clipboardsidebar.h"

#include "definitions/definitions.h"
#include "saurus/plugin-system/clipboard/clipboardmodel.h"
#include "saurus/plugin-system/clipboard/clipboardplugin.h"

#include <QHeaderView>
#include <QTreeView>

ClipboardSidebar::ClipboardSidebar(ClipboardPlugin* plugin, QWidget* parent)
  : BaseSidebar(plugin->textApp(), parent), m_treeClipboard(nullptr), m_model(nullptr) {
  setWindowTitle(tr("Clipboard History"));
  setObjectName(QSL("m_sidebarClipboard"));
}

Qt::DockWidgetArea ClipboardSidebar::initialArea() const {
  return Qt::DockWidgetArea::RightDockWidgetArea;
}

bool ClipboardSidebar::initiallyVisible() const {
  return true;
}

int ClipboardSidebar::initialWidth() const {
  return 150;
}

void ClipboardSidebar::load() {
  if (m_treeClipboard == nullptr) {
    m_treeClipboard = new QTreeView(this);
    m_model = new ClipboardModel(this);
    m_treeClipboard->setModel(m_model);

    m_treeClipboard->setIndentation(0);
    m_treeClipboard->header()->setSectionResizeMode(0, QHeaderView::ResizeMode::ResizeToContents);
    m_treeClipboard->header()->setSectionResizeMode(1, QHeaderView::ResizeMode::Stretch);
  }

  setWidget(m_treeClipboard);
}
