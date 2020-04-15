// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "saurus/miscellaneous/textapplication.h"

// *INDENT-OFF*
#include PATH(SCINTILLA_DIR,include/SciLexer.h)

// *INDENT-ON*

#include "3rd-party/uchardet/uchardet.h"
#include "common/gui/messagebox.h"
#include "common/gui/toolbar.h"
#include "common/miscellaneous/iconfactory.h"
#include "common/miscellaneous/textfactory.h"
#include "definitions/definitions.h"
#include "saurus/external-tools/externaltool.h"
#include "saurus/external-tools/externaltools.h"
#include "saurus/gui/dialogs/formencryptionsettings.h"
#include "saurus/gui/dialogs/formfindreplace.h"
#include "saurus/gui/dialogs/formmain.h"
#include "saurus/gui/editortab.h"
#include "saurus/gui/sidebars/findresultssidebar.h"
#include "saurus/gui/sidebars/outputsidebar.h"
#include "saurus/gui/statusbar.h"
#include "saurus/gui/tabwidget.h"
#include "saurus/gui/texteditor.h"
#include "saurus/miscellaneous/application.h"
#include "saurus/miscellaneous/syntaxhighlighting.h"
#include "saurus/plugin-system/pluginfactory.h"

#include <QDockWidget>
#include <QDropEvent>
#include <QFileDialog>
#include <QIcon>
#include <QLineEdit>
#include <QPointer>
#include <QTemporaryFile>
#include <QTextCodec>
#include <QTimer>

TextApplication::TextApplication(QObject* parent)
  : QObject(parent), m_shouldSaveSession(false), m_settings(new TextApplicationSettings(this)), m_findReplaceDialog(nullptr) {
  m_outputSidebar = new OutputSidebar(this, nullptr);
  m_actionShowOutputSidebar = m_outputSidebar->generateAction();

  m_findResultsSidebar = new FindResultsSidebar(this, nullptr);
  m_actionShowFindResultsSidebar = m_findResultsSidebar->generateAction();

  // Hook ext. tools early.
  connect(m_settings->externalTools(), &ExternalTools::externalToolsChanged, this, &TextApplication::loadNewExternalTools);
}

void TextApplication::loadTextEditorFromString(const QString& contents) {
  auto* new_editor = new TextEditor(this, m_tabEditors);

  if (m_tabEditors->hasOnlyOneEmptyEditor()) {
    // We have one empty non modified editor already open, close it.
    m_tabEditors->closeTab(0);
  }

  attachTextEditor(new_editor);
  m_tabEditors->setCurrentIndex(addTextEditor(new_editor));
  new_editor->loadFromString(contents);
  new_editor->setFocus();
}

TextEditor* TextApplication::loadTextEditorFromFile(const QString& file_path,
                                                    const QString& explicit_encoding,
                                                    const QString& file_filter,
                                                    bool restoring_session) {
  Tab* tab_already_opened_file = m_tabEditors->tabWithFile(file_path);

  if (tab_already_opened_file != nullptr) {
    m_tabEditors->setCurrentWidget(tab_already_opened_file);
    tab_already_opened_file->primaryEditor()->setFocus();
    return tab_already_opened_file->primaryEditor();
  }

  TextEditor* new_editor = TextEditor::fromTextFile(this, file_path, explicit_encoding, file_filter);

  if (new_editor != nullptr) {
    if (!restoring_session && m_tabEditors->hasOnlyOneEmptyEditor()) {
      // We have one empty non modified editor already open, close it.
      m_tabEditors->closeTab(0);
    }

    attachTextEditor(new_editor);

    if (!restoring_session) {
      m_settings->setLoadSaveDefaultDirectory(file_path);
    }

    m_tabEditors->setCurrentIndex(addTextEditor(new_editor));
    new_editor->setFocus();
  }

  return new_editor;
}

int TextApplication::addTextEditor(TextEditor* editor) {
  Tab* tab = new EditorTab(this, editor);

  connect(tab, &Tab::iconChanged, this, &TextApplication::onTabIconChanged);
  connect(tab, &Tab::titleChanged, this, &TextApplication::onTabTitleChanged);

  return m_tabEditors->addTab(tab, QIcon(), tab->title());
}

void TextApplication::attachTextEditor(TextEditor* editor) {
  editor->viewport()->installEventFilter(this);

  connect(editor, &TextEditor::editorReloaded, this, &TextApplication::onEditorReloaded);
  connect(editor, &TextEditor::savedToFile, this, &TextApplication::onEditorSaved);
  connect(editor, &TextEditor::modified, this, &TextApplication::onEditorModified);
}

void TextApplication::processCommandLineArguments() {
  // We load any documents passed as parameters.
  loadFilesFromArgs(qApp->cmdParser()->positionalArguments());

  if (tabWidget()->count() == 0) {
    newFile();
  }
}

void TextApplication::saveCurrentEditor() {
  TextEditor* editor = tabWidget()->currentEditor();

  if (editor != nullptr) {
    bool ok = false;

    editor->save(ok);
  }
}

void TextApplication::saveCurrentEditorAs() {
  TextEditor* editor = tabWidget()->currentEditor();

  if (editor != nullptr) {
    bool ok = false;

    editor->saveAs(ok);
  }
}

void TextApplication::saveCurrentEditorAsWithEncoding(QAction* action) {
  TextEditor* editor = tabWidget()->currentEditor();

  if (editor != nullptr) {
    bool ok = false;
    QString encoding = action->data().toString();

    editor->saveAs(ok, encoding);
  }
}

void TextApplication::saveAllEditors() {
  foreach (TextEditor* editor, m_tabEditors->editors()) {
    bool ok = false;

    editor->save(ok);
  }
}

void TextApplication::closeAllUnmodifiedEditors() {
  foreach (TextEditor* editor, m_tabEditors->editors()) {
    if (!editor->modify()) {
      m_tabEditors->closeTab(m_tabEditors->indexOfEditor(editor));
    }
  }
}

void TextApplication::reloadCurrentEditor() {
  TextEditor* editor = tabWidget()->currentEditor();

  if (editor != nullptr) {
    editor->reloadFromDisk();
  }
}

void TextApplication::reloadEditorsAfterSettingsChanged(bool reload_visible, bool reload_all) {
  foreach (TextEditor* editor, m_tabEditors->editors()) {
    editor->setSettingsDirty(true);
  }

  if (reload_all) {
    foreach (TextEditor* editor, m_tabEditors->editors()) {
      editor->reloadSettings();
    }
  }
  else if (reload_visible) {
    TextEditor* visible = tabWidget()->currentEditor();

    if (visible != nullptr) {
      visible->reloadSettings();
    }
  }
}

void TextApplication::setCurrentEditorAutoIndentEnabled(bool auto_indent_enabled) {
  TextEditor* editor = tabWidget()->currentEditor();

  if (editor != nullptr) {
    editor->setAutoIndentEnabled(auto_indent_enabled);
  }

  m_settings->setAutoIndentEnabled(auto_indent_enabled);
}

void TextApplication::setupEolMenu() {
  TextEditor* editor = tabWidget()->currentEditor();

  if (editor != nullptr) {
    updateEolMenu(int(editor->eOLMode()));
  }
  else {
    updateEolMenu(settings()->eolMode());
  }
}

void TextApplication::updateEolMenu(int eol_mode) {
  if (m_actionEolMac->actionGroup() == nullptr) {
    auto* grp_eol = new QActionGroup(m_menuEolMode);

    grp_eol->addAction(m_actionEolMac);
    grp_eol->addAction(m_actionEolUnix);
    grp_eol->addAction(m_actionEolWindows);
  }

  switch (eol_mode) {
    case SC_EOL_CRLF:
      m_actionEolWindows->setChecked(true);
      break;

    case SC_EOL_LF:
      m_actionEolUnix->setChecked(true);
      break;

    case SC_EOL_CR:
      m_actionEolMac->setChecked(true);
      break;

    default:
      break;
  }
}

void TextApplication::markEditorModified(TextEditor* editor, bool modified) {
  Q_UNUSED(modified)

  if (m_tabEditors->indexOfEditor(editor) >= 0) {
    updateToolBarFromEditor(editor, true);
  }
}

OutputSidebar* TextApplication::outputSidebar() const {
  return m_outputSidebar;
}

FindResultsSidebar* TextApplication::findResultsSidebar() const {
  return m_findResultsSidebar;
}

TextApplicationSettings* TextApplication::settings() const {
  return m_settings;
}

TabWidget* TextApplication::tabWidget() const {
  return m_tabEditors;
}

void TextApplication::loadFilesFromArgs(const QList<QString>& files) {
  foreach (const QString& file_path, files) {
    if (!file_path.startsWith(QL1S("-"))) {
      QTimer::singleShot(0, this, [this, file_path] {
        loadTextEditorFromFile(file_path);
      });
    }
  }
}

void TextApplication::undo() {
  TextEditor* editor = tabWidget()->currentEditor();

  if (editor != nullptr) {
    editor->undo();
  }
}

void TextApplication::redo() {
  TextEditor* editor = tabWidget()->currentEditor();

  if (editor != nullptr) {
    editor->redo();
  }
}

void TextApplication::newFile() {
  auto* editor = new TextEditor(this, m_tabEditors);

  attachTextEditor(editor);

  m_tabEditors->setCurrentIndex(addTextEditor(editor));
}

void TextApplication::onEditorModified(int type, int position, int length,
                                       int linesAdded, const QByteArray& text,
                                       int line, int foldNow, int foldPrev) {
  Q_UNUSED(position)
  Q_UNUSED(length)
  Q_UNUSED(linesAdded)
  Q_UNUSED(text)
  Q_UNUSED(line)
  Q_UNUSED(foldNow)
  Q_UNUSED(foldPrev)

  if ((type & (SC_MOD_INSERTTEXT | SC_MOD_DELETETEXT)) > 0) {
    auto* editor = qobject_cast<TextEditor*>(sender());

    markEditorModified(editor, editor->modify());
  }
}

void TextApplication::onEditorSaved() {
  auto* editor = qobject_cast<TextEditor*>(sender());

  //updateStatusBarFromEditor(editor);

  if (!shouldSaveSession()) {
    m_settings->setLoadSaveDefaultDirectory(editor->filePath());
  }

  if (editor == tabWidget()->currentEditor()) {
    onTabSwitched(m_tabEditors->indexOfEditor(editor));
  }
}

void TextApplication::onEditorReloaded() {
  auto* sndr = qobject_cast<TextEditor*>(sender());

  if (sndr == tabWidget()->currentEditor()) {
    onTabSwitched(m_tabEditors->indexOfEditor(sndr));
  }
}

void TextApplication::createConnections() {
  connect(m_mainForm, &FormMain::closeRequested, this, &TextApplication::quit);

  // Misc connections.
  connect(qApp, &Application::dataSaveRequested, this, &TextApplication::quit);
  connect(m_settings, &TextApplicationSettings::settingsChanged, this, &TextApplication::reloadEditorsAfterSettingsChanged);

  // Tab widget.
  connect(m_tabEditors, &TabWidget::currentChanged, this, &TextApplication::onTabSwitched);
  connect(m_tabEditors->tabBar(), &TabBar::emptySpaceDoubleClicked, this, &TextApplication::newFile);

  // Actions.
  connect(m_actionPrintCurrentEditor, &QAction::triggered, this, &TextApplication::printCurrentEditor);
  connect(m_actionPrintPreviewCurrentEditor, &QAction::triggered, this, &TextApplication::printPreviewCurrentEditor);
  connect(m_actionPrintPreviewCurrentEditorBlackWhite, &QAction::triggered, this, &TextApplication::printPreviewCurrentEditorBlackWhite);
  connect(m_actionFindReplace, &QAction::triggered, this, &TextApplication::showFindReplaceDialog);
  connect(m_actionTabsCloseAllUnmodified, &QAction::triggered, this, &TextApplication::closeAllUnmodifiedEditors);
  connect(m_actionFileSave, &QAction::triggered, this, &TextApplication::saveCurrentEditor);
  connect(m_actionFileSaveAs, &QAction::triggered, this, &TextApplication::saveCurrentEditorAs);
  connect(m_actionFileSaveAll, &QAction::triggered, this, &TextApplication::saveAllEditors);
  connect(m_actionFileReload, &QAction::triggered, this, &TextApplication::reloadCurrentEditor);
  connect(m_actionFileEncryption, &QAction::triggered, this, &TextApplication::setupFileEncryption);
  connect(m_actionFileNew, &QAction::triggered, this, &TextApplication::newFile);
  connect(m_actionFileOpen, &QAction::triggered, this, [this]() {
    openTextFile();
  });
  connect(m_actionWordWrap, &QAction::triggered, m_settings, &TextApplicationSettings::setWordWrapEnabled);
  connect(m_actionContextAwareHighlighting, &QAction::triggered,
          m_settings, &TextApplicationSettings::setContextAwareHighlightingEnabled);
  connect(m_actionCodeFolding, &QAction::triggered, m_settings, &TextApplicationSettings::setCodeFoldingEnabled);
  connect(m_actionLineNumbers, &QAction::triggered, m_settings, &TextApplicationSettings::setLineNumbersEnabled);
  connect(m_actionViewEols, &QAction::triggered, m_settings, &TextApplicationSettings::setViewEols);
  connect(m_actionViewWhitespaces, &QAction::triggered, m_settings, &TextApplicationSettings::setViewWhitespaces);
  connect(m_actionEditBack, &QAction::triggered, this, &TextApplication::undo);
  connect(m_actionEditForward, &QAction::triggered, this, &TextApplication::redo);
  connect(m_actionAutoIndentEnabled, &QAction::triggered, this, &TextApplication::setCurrentEditorAutoIndentEnabled);

  // Menus.
  connect(m_menuEolMode, &QMenu::triggered, this, &TextApplication::changeEolMode);
  connect(m_menuEolMode, &QMenu::aboutToShow, this, &TextApplication::setupEolMenu);
  connect(m_menuEolConversion, &QMenu::triggered, this, &TextApplication::convertEols);
  connect(m_menuFileOpenWithEncoding, &QMenu::aboutToShow, this, [this]() {
    if (m_menuFileOpenWithEncoding->isEmpty()) {
      TextFactory::initializeEncodingMenu(m_menuFileOpenWithEncoding);
    }
  });
  connect(m_menuFileOpenWithEncoding, &QMenu::triggered, this, &TextApplication::openTextFile);

  connect(m_menuFileReopenWithEncoding, &QMenu::aboutToShow, this, [this]() {
    if (m_menuFileReopenWithEncoding->isEmpty()) {
      TextFactory::initializeEncodingMenu(m_menuFileReopenWithEncoding);
    }
  });
  connect(m_menuFileReopenWithEncoding, &QMenu::triggered, this, &TextApplication::reopenTextFile);

  connect(m_menuEncoding, &QMenu::aboutToShow, this, &TextApplication::loadEncodingMenu);
  connect(m_menuEncoding, &QMenu::triggered, this, &TextApplication::changeEncoding);
  connect(m_menuFileSaveWithEncoding, &QMenu::aboutToShow, this, [this]() {
    if (m_menuFileSaveWithEncoding->isEmpty()) {
      TextFactory::initializeEncodingMenu(m_menuFileSaveWithEncoding);
    }
  });
  connect(m_menuFileSaveWithEncoding, &QMenu::triggered, this, &TextApplication::saveCurrentEditorAsWithEncoding);
  connect(m_menuRecentFiles, &QMenu::aboutToShow, this, &TextApplication::fillRecentFiles);
  connect(m_menuLanguage, &QMenu::aboutToShow, this, &TextApplication::loadLexersMenu);
  connect(m_menuLanguage, &QMenu::triggered, this, &TextApplication::changeLexer);
  connect(m_menuRecentFiles, &QMenu::triggered, this, [this](QAction* action) {
    loadTextEditorFromFile(action->text());
  });
}

void TextApplication::setMainForm(FormMain* main_form) {
  m_mainForm = main_form;
  m_mainForm->installEventFilter(this);

  m_tabEditors = main_form->tabWidget();
  m_statusBar = main_form->statusBar();

  // Get pointers to editor-related global actions/menus.
  m_actionNoAction = m_mainForm->m_ui.m_actionNoActions;
  m_actionFileNew = m_mainForm->m_ui.m_actionFileNew;
  m_actionFileOpen = m_mainForm->m_ui.m_actionFileOpen;
  m_actionFileSave = m_mainForm->m_ui.m_actionFileSave;
  m_actionFileSaveAs = m_mainForm->m_ui.m_actionFileSaveAs;
  m_actionFileSaveAll = m_mainForm->m_ui.m_actionFileSaveAll;
  m_actionFileReload = m_mainForm->m_ui.m_actionFileReload;
  m_actionFileEncryption = m_mainForm->m_ui.m_actionFileEncryption;
  m_actionEolUnix = m_mainForm->m_ui.m_actionEolUnix;
  m_actionEolWindows = m_mainForm->m_ui.m_actionEolWindows;
  m_actionEolMac = m_mainForm->m_ui.m_actionEolMac;
  m_actionEolConvertUnix = m_mainForm->m_ui.m_actionEolConvertUnix;
  m_actionEolConvertWindows = m_mainForm->m_ui.m_actionEolConvertWindows;
  m_actionEolConvertMac = m_mainForm->m_ui.m_actionEolConvertMac;
  m_actionFindReplace = m_mainForm->m_ui.m_actionFindReplace;
  m_actionWordWrap = m_mainForm->m_ui.m_actionWordWrap;
  m_actionCodeFolding = m_mainForm->m_ui.m_actionCodeFolding;
  m_actionLineNumbers = m_mainForm->m_ui.m_actionLineNumbers;
  m_actionTabsCloseAllUnmodified = m_mainForm->m_ui.m_actionTabsCloseAllUnmodified;
  m_actionEditBack = m_mainForm->m_ui.m_actionEditBack;
  m_actionEditForward = m_mainForm->m_ui.m_actionEditForward;
  m_actionSettings = m_mainForm->m_ui.m_actionSettings;
  m_actionViewWhitespaces = m_mainForm->m_ui.m_actionViewWhitespaces;
  m_actionViewEols = m_mainForm->m_ui.m_actionViewEols;
  m_actionPrintCurrentEditor = m_mainForm->m_ui.m_actionPrint;
  m_actionPrintPreviewCurrentEditor = m_mainForm->m_ui.m_actionPrintPreview;
  m_actionPrintPreviewCurrentEditorBlackWhite = m_mainForm->m_ui.m_actionPrintPreviewBlackWhite;
  m_actionAutoIndentEnabled = m_mainForm->m_ui.m_actionAutoIndentEnabled;
  m_actionContextAwareHighlighting = m_mainForm->m_ui.m_actionContextAwareHighlighting;

  m_menuView = m_mainForm->m_ui.m_menuView;
  m_menuEdit = m_mainForm->m_ui.m_menuEdit;
  m_menuViewInvisibles = m_mainForm->m_ui.m_menuViewInvisibles;
  m_menuDockWidgets = m_mainForm->m_ui.m_menuDockWidgets;
  m_menuFileSaveWithEncoding = m_mainForm->m_ui.m_menuFileSaveWithEncoding;
  m_menuFileOpenWithEncoding = m_mainForm->m_ui.m_menuFileOpenWithEncoding;
  m_menuFileReopenWithEncoding = m_mainForm->m_ui.m_menuFileReopenWithEncoding;
  m_menuEolMode = m_mainForm->m_ui.m_menuEolMode;
  m_menuEolConversion = m_mainForm->m_ui.m_menuEolConversion;
  m_menuTools = m_mainForm->m_ui.m_menuTools;
  m_menuRecentFiles = m_mainForm->m_ui.m_menuRecentFiles;
  m_menuLanguage = m_mainForm->m_ui.m_menuLanguage;
  m_menuEncoding = m_mainForm->m_ui.m_menuEncoding;

  m_actionEolMac->setData(SC_EOL_CR);
  m_actionEolUnix->setData(SC_EOL_LF);
  m_actionEolWindows->setData(SC_EOL_CRLF);
  m_actionEolConvertMac->setData(SC_EOL_CR);
  m_actionEolConvertUnix->setData(SC_EOL_LF);
  m_actionEolConvertWindows->setData(SC_EOL_CRLF);

  createConnections();
  loadState();
}

QList<QAction*> TextApplication::userActions() const {
  return QList<QAction*>() << m_actionShowFindResultsSidebar
                           << m_actionShowOutputSidebar
                           << settings()->pluginFactory()->assignableActions()
                           << settings()->externalTools()->predefinedToolsActions();
}

void TextApplication::loadState() {
  m_actionWordWrap->setChecked(m_settings->wordWrapEnabled());
  m_actionLineNumbers->setChecked(m_settings->lineNumbersEnabled());
  m_actionCodeFolding->setChecked(m_settings->codeFoldingEnabled());
  m_actionViewEols->setChecked(m_settings->viewEols());
  m_actionContextAwareHighlighting->setChecked(m_settings->contextAwareHighlightingEnabled());
  m_actionViewWhitespaces->setChecked(m_settings->viewWhitespaces());

  m_actionEditBack->setEnabled(false);
  m_actionEditForward->setEnabled(false);

  // Load size/position/visibility of dock widgets.
  m_mainForm->setDockOptions(QMainWindow::DockOption::AnimatedDocks |
                             QMainWindow::DockOption::AllowTabbedDocks | QMainWindow::DockOption::GroupedDragging);
  m_mainForm->setCorner(Qt::Corner::BottomLeftCorner, Qt::DockWidgetArea::LeftDockWidgetArea);
  m_mainForm->setCorner(Qt::Corner::BottomRightCorner, Qt::DockWidgetArea::RightDockWidgetArea);
  m_mainForm->setCorner(Qt::Corner::TopLeftCorner, Qt::DockWidgetArea::LeftDockWidgetArea);
  m_mainForm->setCorner(Qt::Corner::TopRightCorner, Qt::DockWidgetArea::RightDockWidgetArea);

  // We load plugins etc.
  settings()->pluginFactory()->loadPlugins(this);

  // We hook actions for showing sidebars and add built-in sidebars.
  m_menuDockWidgets->addActions(settings()->pluginFactory()->sidebarActions());
  m_menuDockWidgets->addAction(m_actionShowFindResultsSidebar);
  m_menuDockWidgets->addAction(m_actionShowOutputSidebar);

  auto dock_actions = m_menuDockWidgets->actions();

  std::sort(dock_actions.begin(), dock_actions.end(), [](QAction* lhs, QAction* rhs) {
    return lhs->text() < rhs->text();
  });
  m_menuDockWidgets->clear();
  m_menuDockWidgets->addActions(dock_actions);

  // We load GUI state of all sidebars.
  QList<BaseSidebar*> sidebars;
  sidebars << m_outputSidebar << m_findResultsSidebar << settings()->pluginFactory()->sidebars();
  settings()->loadInitialSidebarGuiSettings(m_mainForm, sidebars);

  // Reload external tools, this also reloads
  // "Tools" menu with submenus for plugins.
  m_settings->externalTools()->reloadTools();
}

void TextApplication::quit(bool* ok) {
  beginSavingSession();

  if (shouldSaveSession()) {
    settings()->setRestoredSessionTabIndex(m_tabEditors->currentIndex());
  }

  bool cancel_closing = false;

  // Ask for agreements first.
  for (int i = 0; i < m_tabEditors->count(); i++) {
    Tab* tab = m_tabEditors->tabAt(i);

    if (cancel_closing) {
      tab->primaryEditor()->resetSaveAgreement();
    }
    else {
      tab->primaryEditor()->askForSaveAgreement();

      if (tab->primaryEditor()->currentSaveAgreement() == QMessageBox::StandardButton::Cancel) {
        // Well, cancel closing now.
        cancel_closing = true;
        i = -1;
      }
    }
  }

  if (cancel_closing) {
    *ok = false;
    return;
  }

  while (m_tabEditors->count() > 0) {
    if (!m_tabEditors->closeTab(0)) {
      // User aborted.
      *ok = false;
      return;
    }
  }

  *ok = true;

  endSavingSession();

  settings()->pluginFactory()->quit();
}

bool TextApplication::eventFilter(QObject* obj, QEvent* event) {
  Q_UNUSED(obj)

  if (event->type() == QEvent::Type::Drop) {
    auto* drop_event = dynamic_cast<QDropEvent*>(event);

    if (drop_event == nullptr) {
      return false;
    }

    if (obj->metaObject()->className() == QSL("FormMain")) {
      // We dropped something to some other widget, probably main window.
      if (drop_event->mimeData()->hasText()) {
        if (drop_event->mimeData()->hasUrls()) {
          // We maybe dropped file.
          drop_event->accept();

          for (const QUrl& urll : drop_event->mimeData()->urls()) {
            QString file_path = urll.toLocalFile();

            if (QFile::exists(file_path)) {
              loadTextEditorFromFile(file_path);
            }
          }

          return true;
        }
        else {
          loadTextEditorFromString(drop_event->mimeData()->text());
          return false;
        }
      }
    }
    else {
      // We probably dropped something to text editor widget.
      if (drop_event->mimeData()->hasText()) {
        if (!drop_event->mimeData()->hasUrls()) {
          // Text editor component will consume this, it will insert
          // the dragged text.
          return false;
        }
        else {
          drop_event->accept();

          for (const QUrl& urll : drop_event->mimeData()->urls()) {
            QString file_path = urll.toLocalFile();

            if (QFile::exists(file_path)) {
              loadTextEditorFromFile(file_path);
            }
          }

          return true;
        }
      }
    }
  }

  return false;
}

void TextApplication::setupFileEncryption() {
  TextEditor* editor = tabWidget()->currentEditor();
  FormEncryptionSettings form(editor->encryptionPassword(), qApp->mainFormWidget());

  if (form.exec() == QDialog::DialogCode::Accepted) {
    editor->setEncryptionPassword(form.encryptionPassword());
  }
}

void TextApplication::printPreviewCurrentEditorBlackWhite() {
  TextEditor* editor = tabWidget()->currentEditor();

  if (editor != nullptr) {
    editor->printPreview(true);
  }
}

void TextApplication::printPreviewCurrentEditor() {
  TextEditor* editor = tabWidget()->currentEditor();

  if (editor != nullptr) {
    editor->printPreview(false);
  }
}

void TextApplication::printCurrentEditor() {
  TextEditor* editor = tabWidget()->currentEditor();

  if (editor != nullptr) {
    editor->print(false);
  }
}

void TextApplication::showFindReplaceDialog() {
  if (m_findReplaceDialog == nullptr) {
    m_findReplaceDialog = new FormFindReplace(this, qApp->mainFormWidget());
  }

  m_findReplaceDialog->display();
}

void TextApplication::changeEolMode(QAction* act) {
  TextEditor* editor = tabWidget()->currentEditor();
  int new_mode = act->data().toInt();

  if (editor != nullptr) {
    // We change EOL mode of existing editor.
    editor->setEOLMode(new_mode);

    updateStatusBarFromEditor(editor);

    if (!editor->filePath().isEmpty()) {
      // If user has some editor opened but it is not "saved file",
      // then we make sure that global EOL mode is changed too.
      return;
    }
  }

  // We change default EOL mode for new documents.
  settings()->setEolMode(new_mode);
}

void TextApplication::changeLexer(QAction* act) {
  TextEditor* cur_editor = tabWidget()->currentEditor();

  if (cur_editor != nullptr) {
    auto lexer_act = act->data().value<Lexer>();

    cur_editor->reloadLexer(lexer_act);
    updateStatusBarFromEditor(cur_editor);
  }
}

void TextApplication::fillRecentFiles() {
  qDeleteAll(m_menuRecentFiles->actions());
  m_menuRecentFiles->clear();

  foreach (const QString& file_path, m_settings->recentFiles()) {
    m_menuRecentFiles->addAction(file_path);
  }
}

void TextApplication::loadLexersMenu() {
  if (m_menuLanguage->isEmpty()) {
    m_lexerActions = QList<QAction*>();

    // Fill the menu.
    auto* grp = new QActionGroup(m_menuLanguage);

    QMap<QChar, QList<QAction*>> groups;

    foreach (const Lexer& lex, m_settings->syntaxHighlighting()->lexers()) {
      auto* act = new QAction(QL1S("&") + lex.m_name, m_menuLanguage);

      if (lex.m_code == SCLEX_NULL) {
        // We move "plain text" lexer to first position.
        m_menuLanguage->addAction(act);
        m_menuLanguage->addSeparator();
      }
      else {
        groups[lex.m_name.at(0)].append(act);
      }

      act->setActionGroup(grp);
      act->setCheckable(true);
      act->setData(QVariant::fromValue<Lexer>(lex));

      m_lexerActions.append(act);
    }

    QMapIterator<QChar, QList<QAction*>> i(groups);

    while (i.hasNext()) {
      i.next();

      if (i.value().size() == 1) {
        m_menuLanguage->addAction(i.value().first());
      }
      else {
        QMenu* menu = m_menuLanguage->addMenu(QL1S("&") + QString(i.key()));

        menu->addActions(i.value());
      }
    }
  }

  TextEditor* current_editor = tabWidget()->currentEditor();

  if (current_editor != nullptr) {
    Lexer lexer = current_editor->lexer();

    foreach (QAction* act, m_lexerActions) {
      auto lexer_act = act->data().value<Lexer>();

      if (lexer_act.m_name == lexer.m_name) {
        act->setChecked(true);
        break;
      }
    }
  }
}

void TextApplication::loadEncodingMenu() {
  if (m_menuEncoding->isEmpty()) {
    TextFactory::initializeEncodingMenu(m_menuEncoding, true);
  }

  // Check current.
  TextEditor* curr_editor = tabWidget()->currentEditor();

  if (curr_editor != nullptr) {
    // There is some editor, load its encoding.
    QList<QAction*> act_to_check = m_menuEncoding->actions();

    while (!act_to_check.isEmpty()) {
      QAction* act = act_to_check.takeFirst();

      if (act->data().toString().compare(curr_editor->encoding(), Qt::CaseInsensitive) == 0) {
        act->setChecked(true);
        break;
      }
      else if (act->menu() != nullptr) {
        act_to_check.append(act->menu()->actions());
      }
    }
  }
  else {
    QAction* checked_encoding_act = m_menuEncoding->actions().constFirst()->actionGroup()->checkedAction();

    if (checked_encoding_act != nullptr) {
      checked_encoding_act->setChecked(false);
    }
  }
}

void TextApplication::reopenTextFile(QAction* action) {
  TextEditor* editor = tabWidget()->currentEditor();

  if (editor != nullptr && !editor->modify()) {
    const QString file_path = editor->filePath();

    if (!file_path.isEmpty() && m_tabEditors->closeTab(m_tabEditors->indexOfEditor(editor))) {
      loadTextEditorFromFile(file_path, action->data().toString());
    }
  }
  else {
    QMessageBox::warning(qApp->mainForm(), tr("Unsaved text file"), tr("Save your file first, please"));
  }
}

void TextApplication::openTextFile(QAction* action) {
  QString encoding = (action != nullptr && !action->data().isNull()) ? action->data().toString() : QString();
  QString selected_filter;
  QString file_path = MessageBox::getOpenFileName(qApp->mainFormWidget(), tr("Open File"),
                                                  m_settings->loadSaveDefaultDirectory(),
                                                  m_settings->syntaxHighlighting()->fileFilters(),
                                                  &selected_filter);

  if (!file_path.isEmpty()) {
    loadTextEditorFromFile(file_path, encoding, selected_filter);
  }
}

void TextApplication::updateToolBarFromEditor(TextEditor* editor, bool only_modified) {
  if (editor != nullptr) {
    if (editor == tabWidget()->currentEditor()) {
      // Current editor changed.
      m_actionEditBack->setEnabled(editor->canUndo());
      m_actionEditForward->setEnabled(editor->canRedo());
      m_actionAutoIndentEnabled->setChecked(editor->autoIndentEnabled());

      if (!only_modified) {
        m_actionFileSave->setEnabled(true);
        m_actionFileSaveAs->setEnabled(true);
        m_actionFileReload->setEnabled(true);
        m_actionFileEncryption->setEnabled(true);
        m_menuFileSaveWithEncoding->setEnabled(true);
        m_menuFileReopenWithEncoding->setEnabled(true);
        m_actionFileSaveAll->setEnabled(true);
        m_actionPrintCurrentEditor->setEnabled(true);
        m_actionPrintPreviewCurrentEditor->setEnabled(true);
        m_actionPrintPreviewCurrentEditorBlackWhite->setEnabled(true);
        m_actionFindReplace->setEnabled(true);
        m_menuEolConversion->setEnabled(true);
      }
    }
    else {
      // Some editor changed but it is not currently active one.
    }
  }
  else {
    // There is no editor.
    m_actionFileSave->setEnabled(false);
    m_actionFileSaveAs->setEnabled(false);
    m_actionFileReload->setEnabled(false);
    m_menuFileSaveWithEncoding->setEnabled(false);
    m_menuFileReopenWithEncoding->setEnabled(false);
    m_actionEditBack->setEnabled(false);
    m_actionEditForward->setEnabled(false);
    m_actionAutoIndentEnabled->setChecked(settings()->autoIndentEnabled());
    m_actionFileSaveAll->setEnabled(false);
    m_actionPrintCurrentEditor->setEnabled(false);
    m_actionPrintPreviewCurrentEditor->setEnabled(false);
    m_actionPrintPreviewCurrentEditorBlackWhite->setEnabled(false);
    m_actionFindReplace->setEnabled(false);
    m_menuEolConversion->setEnabled(false);
    m_actionFileEncryption->setEnabled(false);
  }
}

void TextApplication::updateStatusBarFromEditor(TextEditor* editor) {
  if (editor == tabWidget()->currentEditor()) {
    if (editor != nullptr) {
      m_statusBar->setEol(int(editor->eOLMode()));
      m_statusBar->setEncoding(editor->encoding());
      m_statusBar->setFileType(editor->lexer().m_name);
    }
    else {
      m_statusBar->setEol(-1);
      m_statusBar->setEncoding(QString());
      m_statusBar->setFileType(QString());
    }
  }
}

bool TextApplication::shouldSaveSession() const {
  return m_shouldSaveSession;
}

void TextApplication::convertEols(QAction* action) {
  int new_mode = action->data().toInt();
  TextEditor* current_editor = tabWidget()->currentEditor();

  if (current_editor != nullptr) {
    current_editor->convertEOLs(new_mode);

    // We also switch EOL mode for new lines.
    switch (new_mode) {
      case SC_EOL_CR:
        m_actionEolMac->trigger();
        break;

      case SC_EOL_LF:
        m_actionEolUnix->trigger();
        break;

      case SC_EOL_CRLF:
        m_actionEolWindows->trigger();
        break;

      default:
        break;
    }
  }
}

void TextApplication::loadNewExternalTools() {
  // Make sure we reload external tools.
  m_menuTools->clear();
  m_menuTools->addAction(m_actionSettings);
  m_menuTools->addSeparator();
  m_menuTools->addActions(m_settings->pluginFactory()->generateMenusForPlugins(m_menuTools));
  m_menuTools->addSeparator();
  m_menuTools->addActions(m_settings->externalTools()->generateToolsMenuTools(m_menuTools));

  // Load also edit menu.
  m_menuEdit->clear();
  m_menuEdit->addAction(m_actionFindReplace);
  m_menuEdit->addSeparator();
  m_menuEdit->addAction(m_actionEditBack);
  m_menuEdit->addAction(m_actionEditForward);
  m_menuEdit->addSeparator();
  m_menuEdit->addAction(m_actionAutoIndentEnabled);
  m_menuEdit->addSeparator();
  m_menuEdit->addMenu(m_menuEolConversion);
  m_menuEdit->addMenu(m_menuEolMode);
  m_menuEdit->addActions(m_settings->externalTools()->generateEditMenuTools(m_menuEdit));
}

void TextApplication::restoreSession() {
  if (settings()->restorePreviousSession()) {
    // Restore editors.
    int index_to_activate = settings()->restoredSessionTabIndex();
    int editors_added = 0;
    const QStringList& session_files = qApp->settings()->value(GROUP(General), SETTING(General::RestoreSessionFiles)).toStringList();
    const QString& user_data_path = qApp->userDataFolder();

    for (const QString& session_file : session_files) {
      if (session_file.startsWith(QL1S("#"))) {
        // Temporary file.
        TextEditor* editor = loadTextEditorFromFile(user_data_path + QDir::separator() + session_file.mid(1),
                                                    QString(),
                                                    QString(),
                                                    true);

        if (editor != nullptr) {
          // WARNING: We need to make sure that document looks
          // "modified".
          auto rng = editor->textRange(0, 1);

          editor->insertText(0, rng);
          editor->deleteRange(0, 1);
          editor->setFilePath(QString());
          m_tabEditors->changeTitle(m_tabEditors->indexOfEditor(editor), tr("New text file"));

          editors_added++;
        }
        else {
          if (index_to_activate >= (editors_added - 1)) {
            index_to_activate--;
          }
        }
      }
      else {
        // Real file.
        loadTextEditorFromFile(session_file, QString(), QString(), true);
        editors_added++;
      }
    }

    if (IS_IN_ARRAY(index_to_activate, editors_added)) {
      m_tabEditors->setCurrentIndex(index_to_activate);
    }
  }

  qApp->settings()->setValue(GROUP(General), General::RestoreSessionFiles, QStringList());
  removeSessionFiles();
}

void TextApplication::removeSessionFiles() {
  for (const QFileInfo& session_temp_file : QDir(qApp->userDataFolder()).entryInfoList({QString("tab_*_%1.session").arg(OS_ID_LOW)},
                                                                                       QDir::Files,
                                                                                       QDir::Name)) {
    QFile::remove(session_temp_file.absoluteFilePath());
  }
}

void TextApplication::beginSavingSession() {
  qApp->settings()->setValue(GROUP(General), General::RestoreSessionFiles, QStringList());
  m_shouldSaveSession = settings()->restorePreviousSession();
  removeSessionFiles();
}

void TextApplication::endSavingSession() {
  m_shouldSaveSession = false;
}

void TextApplication::onTabSwitched(int index) {
  Tab* tab = m_tabEditors->tabAt(index);
  TextEditor* editor = m_tabEditors->textEditorAt(index);

  if (editor != nullptr) {
    editor->reloadSettings();
  }

  if (tab != nullptr) {
    onTabWithIndexTitleChanged(tab, index, tab->title(), tab->toolTip());
  }

  updateToolBarFromEditor(editor, false);
  updateStatusBarFromEditor(editor);
}

void TextApplication::onTabIconChanged(const QIcon& icon) {
  Tab* tab = qobject_cast<Tab*>(sender());
  int index = m_tabEditors->indexOf(tab);

  if (index >= 0) {
    m_tabEditors->changeIcon(index, icon);
  }
}

void TextApplication::onTabTitleChanged(const QString& title, const QString& tool_tip) {
  Tab* tab = qobject_cast<Tab*>(sender());
  int index = m_tabEditors->indexOf(tab);

  if (index >= 0) {
    onTabWithIndexTitleChanged(tab, index, title, tool_tip);
  }
}

void TextApplication::onTabWithIndexTitleChanged(Tab* tab, int index, const QString& title, const QString& tool_tip) {
  m_tabEditors->changeIcon(index, tab->icon());
  m_tabEditors->changeTitle(index, title, tool_tip);
}

void TextApplication::changeEncoding(QAction* act) {
  TextEditor* editor = tabWidget()->currentEditor();

  if (editor != nullptr) {
    editor->setEncoding(act->data().toString().toUtf8());
    updateStatusBarFromEditor(editor);
  }
}
