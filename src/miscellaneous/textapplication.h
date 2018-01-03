// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef TEXTAPPLICATION_H
#define TEXTAPPLICATION_H

#include <QObject>

#include "definitions/definitions.h"

#include "miscellaneous/textapplicationsettings.h"

class TextEditor;
class TabWidget;
class OutputWindow;
class FormMain;
class StatusBar;
class FormFindReplace;
class ExternalTool;
class FilesystemSidebar;
class QAction;
class QMenu;
class QLineEdit;

// Main class which ties text box functionality into GUI and gels all together.
class TextApplication : public QObject {
  Q_OBJECT

  public:
    explicit TextApplication(QObject* parent = nullptr);
    virtual ~TextApplication() = default;

    TextEditor* currentEditor() const;
    OutputWindow* outputWindow() const;
    TextApplicationSettings* settings() const;

    void setMainForm(FormMain* main_form);

  public slots:
    void undo();
    void redo();

    // New editors.
    int addTextEditor(TextEditor* editor);
    void attachTextEditor(TextEditor* editor);

    // Load/save/new files.
    void newFile();
    void reopenTextFile(QAction* action);
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
    void reloadCurrentEditor();

    // Closes all opened text documents (asks to save them if necessary).
    void quit(bool* ok);

  protected:
    virtual bool eventFilter(QObject* obj, QEvent* event) override;

  private slots:
    void printPreviewCurrentEditor();
    void printCurrentEditor();
    void openFindReplaceDialog();

    // EOL stuff.
    void convertEols(QAction* action);
    void changeEolMode(QAction* act);
    void setupEolMenu();

    // Encoding & lexing & folding.
    void changeEncoding(QAction* act);
    void loadEncodingMenu();
    void changeLexer(QAction* act);
    void loadLexersMenu();
    void fillRecentFiles();

    void onSavePointChanged();
    void onEditorSaved();
    void onEditorReloaded();
    void onEditorRequestedVisibility();

    void onEditorModified(int type, int position, int length, int linesAdded,
                          const QByteArray& text, int line, int foldNow, int foldPrev);
    void onEditorTabSwitched(int index = -1);
    void reloadEditorsAfterSettingsChanged(bool reload_visible, bool reload_all);
    void showTabContextMenu(const QPoint& point);

    // External tools.
    void loadNewExternalTools();

    // Misc.
    void initializeDockWidgetsMenu();

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
    OutputWindow* m_outputSidebar;
    FilesystemSidebar* m_filesystemSidebar;
    FormFindReplace* m_findReplaceDialog;
    FormMain* m_mainForm;

    QList<QAction*> m_lexerActions;

    // Pointers to important GUI elements outside of editors.
    QAction* m_actionNoAction;
    QAction* m_actionFileNew;
    QAction* m_actionFileOpen;
    QAction* m_actionFileSave;
    QAction* m_actionFileSaveAs;
    QAction* m_actionFileSaveAll;
    QAction* m_actionFileReload;
    QAction* m_actionEolUnix;
    QAction* m_actionEolWindows;
    QAction* m_actionEolMac;
    QAction* m_actionEolConvertUnix;
    QAction* m_actionEolConvertWindows;
    QAction* m_actionEolConvertMac;
    QAction* m_actionFindReplace;
    QAction* m_actionTabsCloseAllUnmodified;
    QAction* m_actionWordWrap;
    QAction* m_actionLineNumbers;
    QAction* m_actionEditBack;
    QAction* m_actionEditForward;
    QAction* m_actionSettings;
    QAction* m_actionViewWhitespaces;
    QAction* m_actionViewEols;
    QAction* m_actionPrintCurrentEditor;
    QAction* m_actionPrintPreviewCurrentEditor;
    QAction* m_actionDockShowOutput;
    QAction* m_actionDockShowFilesystem;
    QMenu* m_menuDockWidgets;
    QMenu* m_menuFileSaveWithEncoding;
    QMenu* m_menuFileOpenWithEncoding;
    QMenu* m_menuFileReopenWithEncoding;
    QMenu* m_menuEolMode;
    QMenu* m_menuEolConversion;
    QMenu* m_menuEdit;
    QMenu* m_menuView;
    QMenu* m_menuViewInvisibles;
    QMenu* m_menuTools;
    QMenu* m_menuSearch;
    QMenu* m_menuRecentFiles;
    QMenu* m_menuLanguage;
    QMenu* m_menuEncoding;
};

#endif // TEXTAPPLICATION_H
