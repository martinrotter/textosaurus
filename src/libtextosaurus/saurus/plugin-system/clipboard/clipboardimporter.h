// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifndef CLIPBOARDIMPORTER_H
#define CLIPBOARDIMPORTER_H

#include <QDialog>

#include "ui_clipboardimporter.h"

namespace Ui {
  class ClipboardImporter;
}

class TextEditor;
class ClipboardItem;

class ClipboardImporter : public QDialog {
  Q_OBJECT

  public:
    explicit ClipboardImporter(ClipboardItem* entry, TextEditor* editor, QWidget* parent = nullptr);

  private:
    Ui::ClipboardImporter m_ui = {};
};

#endif // CLIPBOARDIMPORTER_H
