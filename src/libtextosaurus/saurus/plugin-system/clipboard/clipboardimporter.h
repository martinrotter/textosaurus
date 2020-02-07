// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifndef CLIPBOARDIMPORTER_H
#define CLIPBOARDIMPORTER_H

#include <QDialog>

#include "ui_clipboardimporter.h"

namespace Ui {
  class ClipboardImporter;
}

class TextEditor;
class TextApplication;
class ClipboardItem;

class ClipboardImporter : public QDialog {
  Q_OBJECT

  public:
    enum class ImportTarget {
      // Import plain text to editor.
      ImportText,

      // Import HTML script to editor.
      ImportHtml,

      // Import URL to editor.
      ImportUrl,

      // Import raw clipboard data (in Base64 form) to editor.
      ImportBase64ClipboardData,

      // Import raw data of file(s) pointed to by URL (in Base64 form).
      ImportBase64FileUrlData,

      // Save raw clipboard data to file.
      SaveBase64ClipboardData
    };

    explicit ClipboardImporter(ClipboardItem* entry, TextApplication* text_app, TextEditor* editor, QWidget* parent = nullptr);

  private slots:
    void performImport();

  private:
    ImportTarget selectedTarget() const;
    void addSupportedActions();

  private:
    Ui::ClipboardImporter m_ui = {};
    ClipboardItem* m_entry;
    TextApplication* m_textApp;
    TextEditor* m_activeEditor;
};

#endif // CLIPBOARDIMPORTER_H
