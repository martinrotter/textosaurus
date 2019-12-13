// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifndef TEXTAPPLICATION_H
#define TEXTAPPLICATION_H

#include <QObject>

#include "definitions/definitions.h"
#include "saurus/miscellaneous/textapplicationsettings.h"

#include <QIcon>

class TextEditor;
class TabWidget;
class OutputSidebar;
class FindResultsSidebar;
class MacrosSidebar;
class FormMain;
class StatusBar;
class FormFindReplace;
class ExternalTool;
class Tab;
class QAction;
class QMenu;
class QLineEdit;

// Main class which ties text box functionality into GUI and gels all together.
class TEXTOSAURUS_DLLSPEC TextApplication : public QObject {
  Q_OBJECT

  friend class PluginFactory;
  friend class TextEditor;

  public:
    explicit TextApplication(QObject* parent = nullptr);

    OutputSidebar* outputSidebar() const;
    FindResultsSidebar* findResultsSidebar() const;
    TextApplicationSettings* settings() const;
    TabWidget* tabWidget() const;

    QList<QAction*> userActions() const;

    void setMainForm(FormMain* main_form);
    bool shouldSaveSession() const;

  public slots:
    void undo();
    void redo();

    // New editors.
    int addTextEditor(TextEditor* editor);
    void attachTextEditor(TextEditor* editor);

    // Load/save/new files.
    void processCommandLineArguments();
    void newFile();
    void reopenTextFile(QAction* action);
    void openTextFile(QAction* action = nullptr);
    void loadFilesFromArgs(const QList<QString>& files);
    void loadTextEditorFromString(const QString& contents);
    TextEditor* loadTextEditorFromFile(const QString& file_path,
                                       const QString& explicit_encoding = QString(),
                                       const QString& file_filter = QString(),
                                       bool restoring_session = false);

    void saveCurrentEditor();
    void saveCurrentEditorAs();
    void saveCurrentEditorAsWithEncoding(QAction* action);
    void saveAllEditors();
    void closeAllUnmodifiedEditors();
    void reloadCurrentEditor();
    void restoreSession();

    // Closes all opened text documents (asks to save them if necessary).
    void quit(bool* ok);

  protected:
    virtual bool eventFilter(QObject* obj, QEvent* event) override;

  private slots:
    void setupFileEncryption();
    void printPreviewCurrentEditorBlackWhite();
    void printPreviewCurrentEditor();
    void printCurrentEditor();
    void showFindReplaceDialog();

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

    void onEditorSaved();
    void onEditorReloaded();

    void onTabIconChanged(const QIcon& icon);
    void onTabTitleChanged(const QString& title, const QString& tool_tip);
    void onTabWithIndexTitleChanged(Tab* tab, int index, const QString& title, const QString& tool_tip);
    void onEditorModified(int type, int position, int length, int linesAdded,
                          const QByteArray& text, int line, int foldNow, int foldPrev);
    void onTabSwitched(int index = -1);
    void reloadEditorsAfterSettingsChanged(bool reload_visible, bool reload_all);
    void setCurrentEditorAutoIndentEnabled(bool auto_indent_enabled);

    // External tools.
    void loadNewExternalTools();

  private:
    void beginSavingSession();
    void endSavingSession();
    void removeSessionFiles();

    void updateEolMenu(int eol_mode);

    // Loads initial state of text application, including session restoring,
    // setup initial GUI state for actions/toolbar/statusbar etc.
    void loadState();

    void createConnections();
    void markEditorModified(TextEditor* editor, bool modified);
    void updateToolBarFromEditor(TextEditor* editor, bool only_modified);
    void updateStatusBarFromEditor(TextEditor* editor);

  private:
    bool m_shouldSaveSession;
    TextApplicationSettings* m_settings;
    TabWidget* m_tabEditors;
    StatusBar* m_statusBar;
    OutputSidebar* m_outputSidebar;
    FindResultsSidebar* m_findResultsSidebar;
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
    QAction* m_actionFileEncryption;
    QAction* m_actionEolUnix;
    QAction* m_actionEolWindows;
    QAction* m_actionEolMac;
    QAction* m_actionEolConvertUnix;
    QAction* m_actionEolConvertWindows;
    QAction* m_actionEolConvertMac;
    QAction* m_actionFindReplace;
    QAction* m_actionTabsCloseAllUnmodified;
    QAction* m_actionWordWrap;
    QAction* m_actionCodeFolding;
    QAction* m_actionLineNumbers;
    QAction* m_actionEditBack;
    QAction* m_actionEditForward;
    QAction* m_actionSettings;
    QAction* m_actionViewWhitespaces;
    QAction* m_actionViewEols;
    QAction* m_actionContextAwareHighlighting;
    QAction* m_actionPrintCurrentEditor;
    QAction* m_actionPrintPreviewCurrentEditor;
    QAction* m_actionPrintPreviewCurrentEditorBlackWhite;
    QAction* m_actionShowOutputSidebar;
    QAction* m_actionShowFindResultsSidebar;
    QAction* m_actionAutoIndentEnabled;
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
    QMenu* m_menuRecentFiles;
    QMenu* m_menuLanguage;
    QMenu* m_menuEncoding;
};

#endif // TEXTAPPLICATION_H
