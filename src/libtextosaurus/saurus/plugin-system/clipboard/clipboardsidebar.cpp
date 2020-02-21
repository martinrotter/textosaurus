// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "saurus/plugin-system/clipboard/clipboardsidebar.h"

#include "common/miscellaneous/iconfactory.h"
#include "definitions/definitions.h"
#include "saurus/gui/tabwidget.h"
#include "saurus/gui/texteditor.h"
#include "saurus/miscellaneous/textapplication.h"
#include "saurus/plugin-system/clipboard/clipboardimporter.h"
#include "saurus/plugin-system/clipboard/clipboardmodel.h"
#include "saurus/plugin-system/clipboard/clipboardplugin.h"

#include <QAction>
#include <QHeaderView>
#include <QToolBar>
#include <QTreeView>

ClipboardSidebar::ClipboardSidebar(ClipboardPlugin* plugin, QWidget* parent)
  : BaseSidebar(plugin->textApp(), parent), m_treeClipboard(nullptr), m_mainForm(plugin->mainForm()), m_model(new ClipboardModel(this)) {
  setWindowTitle(tr("Clipboard History"));
  setObjectName(QSL("m_sidebarClipboard"));

  m_actionInsertLatestText = new QAction(plugin->iconFactory()->fromTheme(QSL("insert-text")),
                                         tr("Insert Newest Text"),
                                         this);
  m_actionInsertLatestText->setObjectName(QSL("m_actionInsertLatestText"));
  m_actionInsertLatestText->setEnabled(false);

  connect(m_actionInsertLatestText, &QAction::triggered, this, &ClipboardSidebar::importNewestText);
  connect(m_model, &ClipboardModel::clipboardChanged, m_actionInsertLatestText, [=]() {
    m_actionInsertLatestText->setEnabled(true);
  });

  m_model->processClipboardChange();
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
    m_treeClipboard->setModel(m_model);
    m_treeClipboard->setIndentation(0);
    m_treeClipboard->header()->setSectionResizeMode(0, QHeaderView::ResizeMode::ResizeToContents);
    m_treeClipboard->header()->setSectionResizeMode(1, QHeaderView::ResizeMode::ResizeToContents);
    m_treeClipboard->header()->setSectionResizeMode(2, QHeaderView::ResizeMode::Stretch);

    m_treeClipboard->setColumnHidden(0, true);

    connect(m_treeClipboard, &QTreeView::activated, this, &ClipboardSidebar::onEntryActivated);

    auto* widget = new QWidget(this);
    auto* layout = new QVBoxLayout(widget);
    auto* tool_bar = new QToolBar(widget);

    tool_bar->addAction(m_actionInsertLatestText);
    tool_bar->setIconSize(QSize(16, 16));

    layout->setMargin(0);
    layout->addWidget(tool_bar);
    layout->addWidget(m_treeClipboard, 1);

    setWidget(widget);
  }

  BaseSidebar::load();
}

void ClipboardSidebar::onEntryActivated(const QModelIndex& idx) {
  ClipboardItem* item = m_model->itemForIndex(idx);

  if (item != nullptr) {
    // Offer user more actions, insert to editor or copy to clipboard.
    importSelectedClipboardEntry(item, m_textApp->tabWidget()->currentEditor());
  }
}

void ClipboardSidebar::importNewestText() {
  auto* entry = m_model->newestEntry();

  if (entry != nullptr) {
    auto* active_editor = m_textApp->tabWidget()->currentEditor();

    importSelectedClipboardEntry(entry, active_editor);
  }
}

void ClipboardSidebar::importSelectedClipboardEntry(ClipboardItem* entry, TextEditor* editor) {
  ClipboardImporter(entry, m_textApp, editor, m_mainForm).exec();
}
