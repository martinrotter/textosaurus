// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef TEXTAPPLICATION_H
#define TEXTAPPLICATION_H

#include <QObject>

#include "definitions/definitions.h"

#include "miscellaneous/textapplicationsettings.h"

class TextEditor;
class TabWidget;
class ToolBox;
class FormMain;
class StatusBar;
class QAction;
class QMenu;

// Main class which ties text box functionality into GUI and gels all together.
class TextApplication : public QObject {
  Q_OBJECT

  public:
    explicit TextApplication(QObject* parent = nullptr);

    TextEditor* currentEditor() const;

    QList<TextEditor*> editors()  const;
    bool anyModifiedEditor() const;
    void setMainForm(FormMain* main_form, TabWidget* tab_widget, StatusBar* status_bar, ToolBox* tool_box);
    TextApplicationSettings& settings();

  public slots:
    void loadFilesFromArgs(int argc, char* argv[]);
    void undo();
    void redo();
    void newFile();

    // Loads initial state of text application, including session restoring,
    // setup initial GUI state for actions/toolbar/statusbar etc.
    void loadState();

    // Closes all opened text documents (asks to save them if necessary).
    void quit(bool* ok);

    void openTextFile(QAction* action = nullptr);
    void loadTextEditorFromFile(const QString& file_path, const QString& explicit_encoding = QString());
    TextEditor* addEmptyTextEditor();

    void saveCurrentEditor();
    void saveCurrentEditorAs();
    void saveCurrentEditorAsWithEncoding(QAction* action);
    void saveAllEditors();
    void closeAllUnmodifiedEditors();

  private slots:
    void reloadEditorsAfterSettingsChanged(bool reload_visible, bool reload_all);
    void onEditorRequestVisibility();
    void onEditorSavedToFile();
    void onEditorLoadedFromFile();
    void onEditorTextChanged(bool modified);
    void onEditorTabSwitched(int index = -1);
    void updateToolBarFromEditor(TextEditor* editor, bool only_modified);
    void updateStatusBarFromEditor(TextEditor* editor);

  private:
    void createConnections();
    void renameEditor(TextEditor* editor);
    void markEditorModified(TextEditor* editor, bool modified);

  private:
    TextApplicationSettings m_settings;
    TabWidget* m_tabEditors;
    StatusBar* m_statusBar;
    ToolBox* m_toolBox;

    // Pointers to important GUI elements outside of editors.
    QAction* m_actionFileNew;
    QAction* m_actionFileOpen;
    QAction* m_actionFileSave;
    QAction* m_actionFileSaveAs;
    QAction* m_actionFileSaveAll;
    QAction* m_actionEolUnix;
    QAction* m_actionEolWindows;
    QAction* m_actionEolMac;
    QAction* m_actionTabsCloseAllUnmodified;
    QAction* m_actionWordWrap;
    QAction* m_actionEditBack;
    QAction* m_actionEditForward;
    QMenu* m_menuFileSaveWithEncoding;
    QMenu* m_menuFileOpenWithEncoding;
    QMenu* m_menuEolMode;
};

#endif // TEXTAPPLICATION_H
