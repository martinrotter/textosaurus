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
class ExternalTool;
class QAction;
class QMenu;
class QLineEdit;

// Main class which ties text box functionality into GUI and gels all together.
class TextApplication : public QObject {
  Q_OBJECT

  public:
    explicit TextApplication(QObject* parent = nullptr);
    virtual ~TextApplication();

    TextEditor* currentEditor() const;
    ToolBox* toolBox() const;
    TextApplicationSettings* settings() const;

    void setMainForm(FormMain* main_form, TabWidget* tab_widget, StatusBar* status_bar, ToolBox* tool_box);

  public slots:

    // Redo/undo.
    void undo();
    void redo();

    // New editors.
    int addTextEditor(TextEditor* editor);
    TextEditor* createTextEditor();

    // Load/save/new files.
    void newFile();
    void openTextFile(QAction* action = nullptr);
    void loadFilesFromArgs(const QList<QString>& files);
    void loadTextEditorFromString(const QString& contents);
    void loadTextEditorFromFile(const QString& file_path,
                                const QString& explicit_encoding = QString(),
                                const QString& file_filter = QString());
    void saveCurrentEditor();
    void saveCurrentEditorAs();
    void saveCurrentEditorAsWithEncoding(QAction* action);
    void saveAllEditors();
    void closeAllUnmodifiedEditors();

    // Closes all opened text documents (asks to save them if necessary).
    void quit(bool* ok);

  protected:
    bool eventFilter(QObject* obj, QEvent* event);

  private slots:

    // EOL stuff.
    void convertEols(QAction* action);
    void changeEolMode(QAction* act);
    void setupEolMenu();

    // Encoding & lexing & folding.
    void changeEncoding(QAction* act);
    void loadEncodingMenu();
    void changeLexer(QAction* act);
    void filterLexersMenu(const QString& filter);
    void loadLexersMenu();
    void fillRecentFiles();

    void onSavePointChanged();
    void onEditorSaved();
    void onEditorRequestedVisibility();

    void onEditorModified(int type, int position, int length, int linesAdded,
                          const QByteArray& text, int line, int foldNow, int foldPrev);
    void onEditorTabSwitched(int index = -1);
    void reloadEditorsAfterSettingsChanged(bool reload_visible, bool reload_all);

    // External tools.
    void loadNewExternalTools();

  private:
    void updateEolMenu(int eol_mode);

    // Loads initial state of text application, including session restoring,
    // setup initial GUI state for actions/toolbar/statusbar etc.
    void loadState();

    void createConnections();
    void renameEditor(TextEditor* editor);
    void updateEditorIcon(int index, bool modified);
    void markEditorModified(TextEditor* editor, bool modified);
    void updateToolBarFromEditor(TextEditor* editor, bool only_modified);
    void updateStatusBarFromEditor(TextEditor* editor);

  private:
    TextApplicationSettings* m_settings;
    TabWidget* m_tabEditors;
    StatusBar* m_statusBar;
    ToolBox* m_toolBox;
    QLineEdit* m_txtLexerFilter;

    // Pointers to important GUI elements outside of editors.
    QAction* m_actionFileNew;
    QAction* m_actionFileOpen;
    QAction* m_actionFileSave;
    QAction* m_actionFileSaveAs;
    QAction* m_actionFileSaveAll;
    QAction* m_actionEolUnix;
    QAction* m_actionEolWindows;
    QAction* m_actionEolMac;
    QAction* m_actionEolConvertUnix;
    QAction* m_actionEolConvertWindows;
    QAction* m_actionEolConvertMac;
    QAction* m_actionTabsCloseAllUnmodified;
    QAction* m_actionWordWrap;
    QAction* m_actionEditBack;
    QAction* m_actionEditForward;
    QAction* m_actionSettings;
    QAction* m_actionViewWhitespaces;
    QAction* m_actionViewEols;
    QMenu* m_menuFileSaveWithEncoding;
    QMenu* m_menuFileOpenWithEncoding;
    QMenu* m_menuEolMode;
    QMenu* m_menuEolConversion;
    QMenu* m_menuTools;
    QMenu* m_menuRecentFiles;
    QMenu* m_menuLanguage;
    QMenu* m_menuEncoding;
};

#endif // TEXTAPPLICATION_H
