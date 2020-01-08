// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "saurus/plugin-system/clipboard/clipboardimporter.h"

#include "saurus/gui/texteditor.h"
#include "saurus/plugin-system/clipboard/clipboardmodel.h"

ClipboardImporter::ClipboardImporter(ClipboardItem* entry, TextEditor* editor, QWidget* parent) : QDialog(parent) {
  m_ui.setupUi(this);
}
