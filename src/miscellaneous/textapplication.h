// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef TEXTAPPLICATION_H
#define TEXTAPPLICATION_H

#include <QObject>

#include "definitions/definitions.h"

class TextEditor;
class TabWidget;
class FormMain;
class QAction;

// Main class which ties text box functionality into GUI and gels all together.
class TextApplication : public QObject {
  Q_OBJECT

  public:
    explicit TextApplication(QObject* parent = nullptr);

    TextEditor* currentEditor() const;

    QList<TextEditor*> editors()  const;
    bool anyModifiedEditor() const;

    void setMainForm(FormMain* main_form);

  public slots:

    // Closes all opened text documents (asks to save them if necessary).
    void quit(bool* ok);
    void openTextFile(QAction* action = nullptr);
    void loadTextEditorFromFile(const QString& file_path, const QString& encoding = QSL(DEFAULT_TEXT_FILE_ENCODING));
    TextEditor* addEmptyTextEditor();

  private slots:
    void onEditorRequestVisibility();
    void onEditorSavedToFile();
    void onEditorLoadedFromFile();
    void onEditorTextChanged(bool modified);
    void onEditorTabSwitched(int index = -1);
    void updateToolBarFromEditor(TextEditor* editor, bool only_modified);

  private:
    void renameEditor(TextEditor* editor);
    void markEditorModified(TextEditor* editor, bool modified);

  private:
    TabWidget* m_tabWidget;
    FormMain* m_mainForm;
};

#endif // TEXTAPPLICATION_H
