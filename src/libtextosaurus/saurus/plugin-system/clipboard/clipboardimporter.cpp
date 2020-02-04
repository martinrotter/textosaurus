// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "saurus/plugin-system/clipboard/clipboardimporter.h"

#include "saurus/gui/texteditor.h"
#include "saurus/plugin-system/clipboard/clipboardmodel.h"

ClipboardImporter::ClipboardImporter(ClipboardItem* entry, TextEditor* editor, QWidget* parent)
  : QDialog(parent), m_entry(entry), m_activeEditor(editor) {
  m_ui.setupUi(this);

  addSupportedActions();
  adjustSize();
}

void ClipboardImporter::addSupportedActions() {
  if (m_entry->data()->hasText()) {
    m_ui.m_cmbOptions->addItem(tr("Import plain text"), int(ImportTarget::ImportText));
  }

  if (m_entry->data()->hasHtml()) {
    m_ui.m_cmbOptions->addItem(tr("Import HTML"), int(ImportTarget::ImportHtml));
  }

  if (m_entry->data()->hasUrls()) {
    m_ui.m_cmbOptions->addItem(tr("Import URL link(s) or file path(s)"), int(ImportTarget::ImportUrl));

    if (m_entry->type() == ClipboardItem::ItemType::File) {
      m_ui.m_cmbOptions->addItem(tr("Import Base64 file(s) data"), int(ImportTarget::ImportBase64FileUrlData));
    }
  }

  m_ui.m_cmbOptions->addItem(tr("Import raw clipboard data as Base64"), int(ImportTarget::ImportBase64ClipboardData));
  m_ui.m_cmbOptions->addItem(tr("Save raw clipboard data to file"), int(ImportTarget::SaveBase64ClipboardData));
}
