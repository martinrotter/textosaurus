// For license of this file, see <project-root-folder>/LICENSE.md.

#include "miscellaneous/textapplication.h"

#include "external-tools/externaltool.h"
#include "external-tools/externaltools.h"
#include "gui/dialogs/formmain.h"
#include "gui/messagebox.h"
#include "gui/statusbar.h"
#include "gui/tabwidget.h"
#include "gui/texteditor.h"
#include "gui/toolbar.h"
#include "gui/toolbox.h"
#include "miscellaneous/syntaxhighlighting.h"
#include "miscellaneous/textfactory.h"

#include "uchardet/uchardet.h"

#include <QFileDialog>
#include <QPointer>
#include <QTemporaryFile>
#include <QTextCodec>
#include <QTimer>

TextApplication::TextApplication(QObject* parent) : QObject(parent), m_settings(new TextApplicationSettings(this)) {
  // Hook ext. tools early.
  connect(m_settings->externalTools(), &ExternalTools::externalToolsChanged, this, &TextApplication::loadNewExternalTools);
  connect(m_settings->externalTools(), &ExternalTools::toolFinished, this, &TextApplication::onExternalToolFinished);
}

TextApplication::~TextApplication() {
  qDebug("Destroying TextApplication");
}

TextEditor* TextApplication::currentEditor() const {
  return m_tabEditors->textEditorAt(m_tabEditors->currentIndex());
}

QList<TextEditor*> TextApplication::editors() const {
  QList<TextEditor*> editors;

  if (m_tabEditors != nullptr) {
    for (int i = 0; i < m_tabEditors->count(); i++) {
      TextEditor* edit = m_tabEditors->textEditorAt(i);

      if (edit != nullptr) {
        editors.append(edit);
      }
    }
  }

  return editors;
}

bool TextApplication::anyModifiedEditor() const {
  foreach (const TextEditor* editor, editors()) {
    if (editor->modify()) {
      return true;
    }
  }

  return false;
}

void TextApplication::loadTextEditorFromFile(const QString& file_path,
                                             const QString& explicit_encoding,
                                             const QString& file_filter) {
  QString encoding;
  Lexer default_lexer;

  if (explicit_encoding.isEmpty()) {
    qDebug("No explicit encoding for file '%s'. Try to detect one.", qPrintable(file_path));

    if ((encoding = TextFactory::detectEncoding(file_path)).isEmpty()) {
      // No encoding auto-detected.
      encoding = QSL(DEFAULT_TEXT_FILE_ENCODING);
      qWarning("Auto-detection of encoding failed, using default encoding.");
    }
    else {
      qDebug("Auto-detected encoding is '%s'.", qPrintable(encoding));
    }
  }
  else {
    encoding = explicit_encoding;
  }

  QFile file(file_path);

  if (!file.exists()) {
    qWarning("File '%s' does not exist and cannot be opened.", qPrintable(file_path));
    return;
  }

  if (file.size() >= MAX_TEXT_FILE_SIZE) {
    QMessageBox::critical(qApp->mainFormWidget(), tr("Cannot open file"),
                          tr("File '%1' too big. %2 can only open files smaller than %3 MB.").arg(QDir::toNativeSeparators(file_path),
                                                                                                  QSL(APP_NAME),
                                                                                                  QString::number(MAX_TEXT_FILE_SIZE /
                                                                                                                  1000000.0)));
    return;
  }

  if (!file.open(QIODevice::OpenModeFlag::ReadOnly)) {
    QMessageBox::critical(qApp->mainFormWidget(), tr("Cannot read file"),
                          tr("File '%1' cannot be opened for reading. Insufficient permissions.").arg(QDir::toNativeSeparators(file_path)));
    return;
  }

  if (file.size() > BIG_TEXT_FILE_SIZE) {
    if (encoding != QSL(DEFAULT_TEXT_FILE_ENCODING) &&
        MessageBox::show(qApp->mainFormWidget(), QMessageBox::Question, tr("Opening big file"),
                         tr("You want to open big text file in encoding which is different from %1. This operation "
                            "might take quite some time.").arg(QSL(DEFAULT_TEXT_FILE_ENCODING)),
                         tr("Do you really want to open the file?"),
                         file.fileName(), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::No) {
      return;
    }
    else {
      // File is quite big, we turn some features off to make sure it loads faster.
      QMessageBox::warning(qApp->mainFormWidget(), tr("Loading big file"),
                           tr("File '%1' is big. %2 will switch some features (for example 'Word wrap') off to "
                              "make sure that file loading is not horribly slow.").arg(QDir::toNativeSeparators(file_path),
                                                                                       QSL(APP_NAME)));
      m_actionWordWrap->setChecked(false);
    }
  }
  else {
    // We try to detect default lexer.
    default_lexer = m_settings->syntaxHighlighting()->lexerForFile(file_path);
  }

  TextEditor* new_editor = createTextEditor();

  new_editor->loadFromFile(file, encoding, default_lexer);

  m_settings->setLoadSaveDefaultDirectory(file_path);
  m_tabEditors->setCurrentIndex(addTextEditor(new_editor));
}

int TextApplication::addTextEditor(TextEditor* editor) {
  return m_tabEditors->addTab(editor, QIcon(), tr("New text file"), TabBar::TabType::TextEditor);
}

TextEditor* TextApplication::createTextEditor() {
  TextEditor* editor = new TextEditor(this, m_tabEditors);

  connect(editor, &TextEditor::modified, this, &TextApplication::onEditorModified);

  // TODO: docasne zakazat asi neni potreba, protoze inicializace
  // se provede po prepnuti indexu tabwidgetu - vcetne prejmenovani
  //connect(editor, &TextEditor::loadedFromFile, this, &TextApplication::onEditorLoadedFromFile);
  connect(editor, &TextEditor::requestVisibility, this, &TextApplication::onEditorRequestVisibility);

  // TODO: mozna neni potreba, toto ted obstarava signal modified - viz vise
  //connect(editor, &TextEditor::textChanged, this, &TextApplication::onEditorTextChanged);

  return editor;
}

void TextApplication::saveCurrentEditor() {
  TextEditor* editor = currentEditor();

  if (editor != nullptr) {
    bool ok;

    editor->save(&ok);
  }
}

void TextApplication::saveCurrentEditorAs() {
  TextEditor* editor = currentEditor();

  if (editor != nullptr) {
    bool ok;

    editor->saveAs(&ok);
  }
}

void TextApplication::saveCurrentEditorAsWithEncoding(QAction* action) {
  TextEditor* editor = currentEditor();

  if (editor != nullptr) {
    bool ok;
    QString encoding = action->data().toString();

    editor->saveAs(&ok, encoding);
  }
}

void TextApplication::saveAllEditors() {
  bool ok;

  foreach (TextEditor* editor, editors()) {
    editor->save(&ok);
  }
}

void TextApplication::closeAllUnmodifiedEditors() {
  foreach (TextEditor* editor, editors()) {
    if (!editor->modify()) {
      m_tabEditors->closeTab(m_tabEditors->indexOf(editor));
    }
  }
}

void TextApplication::reloadEditorsAfterSettingsChanged(bool reload_visible, bool reload_all) {
  if (reload_all) {
    foreach (TextEditor* editor, editors()) {
      editor->reloadSettings();
    }
  }
  else if (reload_visible) {
    TextEditor* visible = currentEditor();

    if (visible != nullptr) {
      visible->reloadSettings();
    }
  }
}

void TextApplication::onEditorRequestVisibility() {
  TextEditor* editor = qobject_cast<TextEditor*>(sender());

  if (editor != nullptr) {
    m_tabEditors->setCurrentWidget(editor);
  }
}

void TextApplication::onEditorLoadedFromFile() {
  TextEditor* editor = qobject_cast<TextEditor*>(sender());

  renameEditor(editor);
  updateToolBarFromEditor(editor, true);
  updateStatusBarFromEditor(editor);
}

void TextApplication::markEditorModified(TextEditor* editor, bool modified) {
  int index = m_tabEditors->indexOf(editor);

  if (index >= 0) {
    m_tabEditors->tabBar()->setTabIcon(index, modified ?
                                       qApp->icons()->fromTheme(QSL("dialog-warning")) :
                                       QIcon());

    renameEditor(editor);
    updateToolBarFromEditor(editor, true);
    updateStatusBarFromEditor(editor);
  }
}

ToolBox* TextApplication::toolBox() const {
  return m_toolBox;
}

void TextApplication::runSelectedExternalTool() {
  TextEditor* editor = currentEditor();

  if (editor != nullptr) {
    ExternalTool* tool_to_run = qobject_cast<QAction*>(sender())->data().value<ExternalTool*>();

    m_toolBox->displayOutput(OutputSource::ExternalTool, QString("Running '%1' tool...").arg(tool_to_run->name()));
    m_settings->externalTools()->runTool(tool_to_run, editor);
  }
}

TextApplicationSettings* TextApplication::settings() const {
  return m_settings;
}

void TextApplication::loadFilesFromArgs(const QList<QString>& files) {
  foreach (const QString& file_path, files) {
    QTimer::singleShot(0, this, [this, file_path] {
      loadTextEditorFromFile(file_path);
    });
  }
}

void TextApplication::undo() {
  TextEditor* editor = currentEditor();

  if (editor != nullptr) {
    editor->undo();
  }
}

void TextApplication::redo() {
  TextEditor* editor = currentEditor();

  if (editor != nullptr) {
    editor->redo();
  }
}

void TextApplication::newFile() {
  TextEditor* editor = createTextEditor();

  // NOTE: Some properties will get loaded when switched to
  // editor. Load rest of those.
  //editor->reloadLexer(m_settings->syntaxHighlighting()->defaultLexer());

  m_tabEditors->setCurrentIndex(addTextEditor(editor));
}

void TextApplication::onEditorModified() {
  TextEditor* editor = qobject_cast<TextEditor*>(sender());

  // TODO: tento slot je navazan na signal modified
  // kterej ma nekolik parametru, mozna nema cenu reagovat na vsechny zmeny
  markEditorModified(editor, editor->modify());
}

void TextApplication::createConnections() {
  // Misc connections.
  connect(qApp, &Application::dataSaveRequested, this, &TextApplication::quit);
  connect(m_settings, &TextApplicationSettings::settingsChanged, this, &TextApplication::reloadEditorsAfterSettingsChanged);

  // Tab widget.
  connect(m_tabEditors, &TabWidget::currentChanged, this, &TextApplication::onEditorTabSwitched);
  connect(m_tabEditors->tabBar(), &TabBar::emptySpaceDoubleClicked, this, &TextApplication::newFile);

  // Actions and menus.
  connect(m_menuEolMode, &QMenu::triggered, m_settings, &TextApplicationSettings::setEolModeFromAction);
  connect(m_menuEolConversion, &QMenu::triggered, this, &TextApplication::convertEols);
  connect(m_actionTabsCloseAllUnmodified, &QAction::triggered, this, &TextApplication::closeAllUnmodifiedEditors);
  connect(m_actionFileSave, &QAction::triggered, this, &TextApplication::saveCurrentEditor);
  connect(m_actionFileSaveAs, &QAction::triggered, this, &TextApplication::saveCurrentEditorAs);
  connect(m_actionFileSaveAll, &QAction::triggered, this, &TextApplication::saveAllEditors);
  connect(m_actionFileNew, &QAction::triggered, this, &TextApplication::newFile);
  connect(m_actionFileOpen, &QAction::triggered, this, [this]() {
    openTextFile();
  });
  connect(m_actionWordWrap, &QAction::toggled, m_settings, &TextApplicationSettings::setWordWrapEnabled);
  connect(m_actionViewEols, &QAction::toggled, m_settings, &TextApplicationSettings::setViewEols);
  connect(m_actionViewWhitespaces, &QAction::toggled, m_settings, &TextApplicationSettings::setViewWhitespaces);
  connect(m_actionEditBack, &QAction::triggered, this, &TextApplication::undo);
  connect(m_actionEditForward, &QAction::triggered, this, &TextApplication::redo);

  connect(m_menuFileOpenWithEncoding, &QMenu::aboutToShow, this, [this]() {
    if (m_menuFileOpenWithEncoding->isEmpty()) {
      TextFactory::initializeEncodingMenu(m_menuFileOpenWithEncoding);
    }
  });
  connect(m_menuFileOpenWithEncoding, &QMenu::triggered, this, &TextApplication::openTextFile);

  connect(m_menuFileSaveWithEncoding, &QMenu::aboutToShow, this, [this]() {
    if (m_menuFileSaveWithEncoding->isEmpty()) {
      TextFactory::initializeEncodingMenu(m_menuFileSaveWithEncoding);;
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

void TextApplication::setMainForm(FormMain* main_form, TabWidget* tab_widget,
                                  StatusBar* status_bar, ToolBox* tool_box) {
  m_tabEditors = tab_widget;
  m_statusBar = status_bar;
  m_toolBox = tool_box;

  // Get pointers to editor-related global actions/menus.
  m_actionFileNew = main_form->m_ui.m_actionFileNew;
  m_actionFileOpen = main_form->m_ui.m_actionFileOpen;
  m_actionFileSave = main_form->m_ui.m_actionFileSave;
  m_actionFileSaveAs = main_form->m_ui.m_actionFileSaveAs;
  m_actionFileSaveAll = main_form->m_ui.m_actionFileSaveAll;
  m_actionEolUnix = main_form->m_ui.m_actionEolUnix;
  m_actionEolWindows = main_form->m_ui.m_actionEolWindows;
  m_actionEolMac = main_form->m_ui.m_actionEolMac;
  m_actionEolConvertUnix = main_form->m_ui.m_actionEolConvertUnix;
  m_actionEolConvertWindows = main_form->m_ui.m_actionEolConvertWindows;
  m_actionEolConvertMac = main_form->m_ui.m_actionEolConvertMac;
  m_actionWordWrap = main_form->m_ui.m_actionWordWrap;
  m_actionTabsCloseAllUnmodified = main_form->m_ui.m_actionTabsCloseAllUnmodified;
  m_actionEditBack = main_form->m_ui.m_actionEditBack;
  m_actionEditForward = main_form->m_ui.m_actionEditForward;
  m_actionSettings = main_form->m_ui.m_actionSettings;
  m_actionViewWhitespaces = main_form->m_ui.m_actionViewWhitespaces;
  m_actionViewEols = main_form->m_ui.m_actionViewEols;

  m_menuFileSaveWithEncoding = main_form->m_ui.m_menuFileSaveWithEncoding;
  m_menuFileOpenWithEncoding = main_form->m_ui.m_menuFileOpenWithEncoding;
  m_menuEolMode = main_form->m_ui.m_menuEolMode;
  m_menuEolConversion = main_form->m_ui.m_menuEolConversion;
  m_menuTools = main_form->m_ui.m_menuTools;
  m_menuRecentFiles = main_form->m_ui.m_menuRecentFiles;
  m_menuLanguage = main_form->m_ui.m_menuLanguage;

  m_actionEolMac->setData(SC_EOL_CR);
  m_actionEolUnix->setData(SC_EOL_LF);
  m_actionEolWindows->setData(SC_EOL_CRLF);
  m_actionEolConvertMac->setData(SC_EOL_CR);
  m_actionEolConvertUnix->setData(SC_EOL_LF);
  m_actionEolConvertWindows->setData(SC_EOL_CRLF);

  connect(main_form, &FormMain::closeRequested, this, &TextApplication::quit);

  loadState();
  createConnections();

  // Make sure that toolbar/statusbar is updated.
  onEditorTabSwitched();
}

void TextApplication::loadState() {
  m_actionEditBack->setEnabled(false);
  m_actionEditForward->setEnabled(false);
  m_actionWordWrap->setChecked(m_settings->wordWrapEnabled());
  m_actionViewEols->setChecked(m_settings->viewEols());
  m_actionViewWhitespaces->setChecked(m_settings->viewWhitespaces());

  // Setup GUI of actions.
  switch (m_settings->eolMode()) {
    case SC_EOL_CR:
      m_actionEolMac->setChecked(true);
      break;

    case SC_EOL_CRLF:
      m_actionEolWindows->setChecked(true);
      break;

    case SC_EOL_LF:
    default:
      m_actionEolUnix->setChecked(true);
      break;
  }

  m_settings->externalTools()->reloadTools();
}

void TextApplication::quit(bool* ok) {
  while (m_tabEditors->count() > 0) {
    if (!m_tabEditors->closeTab(0)) {
      // User aborted.
      *ok = false;
      return;
    }
  }

  *ok = true;
}

void TextApplication::changeLexer(QAction* act) {
  TextEditor* cur_editor = currentEditor();

  if (cur_editor != nullptr) {
    Lexer lexer_act = act->data().value<Lexer>();

    cur_editor->reloadLexer(lexer_act);
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
    // Fill the menu.
    QActionGroup* grp = new QActionGroup(m_menuLanguage);

    foreach (const Lexer& lex, m_settings->syntaxHighlighting()->lexers()) {
      QAction* act = m_menuLanguage->addAction(QL1S("&") + lex.m_name);

      act->setActionGroup(grp);
      act->setCheckable(true);
      act->setData(QVariant::fromValue<Lexer>(lex));
    }
  }

  TextEditor* current_editor = currentEditor();

  if (current_editor != nullptr) {
    Lexer lexer = current_editor->lexer();

    foreach (QAction* act, m_menuLanguage->actions()) {
      Lexer lexer_act = act->data().value<Lexer>();

      if (lexer_act.m_name == lexer.m_name) {
        act->setChecked(true);
        break;
      }
    }
  }
}

void TextApplication::openTextFile(QAction* action) {
  QString encoding = (action != nullptr && !action->data().isNull()) ? action->data().toString() : QString();
  QString selected_filter;
  QString file_path = QFileDialog::getOpenFileName(qApp->mainFormWidget(), tr("Open file"),
                                                   m_settings->loadSaveDefaultDirectory(),
                                                   m_settings->syntaxHighlighting()->fileFilters().join(QSL(";;")),
                                                   &selected_filter);

  if (!file_path.isEmpty()) {
    loadTextEditorFromFile(file_path, encoding, selected_filter);
  }
}

void TextApplication::onEditorTabSwitched(int index) {
  TextEditor* editor = m_tabEditors->textEditorAt(index);

  if (editor != nullptr) {
    editor->reloadSettings();
  }

  renameEditor(editor);
  updateToolBarFromEditor(editor, false);
  updateStatusBarFromEditor(editor);
}

void TextApplication::onEditorTextChanged() {
  TextEditor* editor = qobject_cast<TextEditor*>(sender());

  if (editor != nullptr) {
    // TODO: docasne vypnuto na zkousku esi je to fakt třeba
    //markEditorModified(editor, editor->isModified());
  }
}

void TextApplication::updateToolBarFromEditor(TextEditor* editor, bool only_modified) {
  Q_UNUSED(only_modified)

  if (editor == currentEditor()) {
    // Current editor is changed or there is now editor at all.

    if (editor != nullptr) {
      // Current editor is changed.
      m_actionFileSave->setEnabled(editor->modify());
      m_actionFileSaveAs->setEnabled(true);
      m_menuFileSaveWithEncoding->setEnabled(true);

      m_actionEditBack->setEnabled(editor->canUndo());
      m_actionEditForward->setEnabled(editor->canRedo());
    }
    else {
      // No editor selected.
      m_actionFileSave->setEnabled(false);
      m_actionFileSaveAs->setEnabled(false);
      m_menuFileSaveWithEncoding->setEnabled(false);

      m_actionEditBack->setEnabled(false);
      m_actionEditForward->setEnabled(false);
    }
  }

  // Enable this if there is at least one unsaved editor.
  m_actionFileSaveAll->setEnabled(anyModifiedEditor());
}

void TextApplication::updateStatusBarFromEditor(TextEditor* editor) {
  if (editor == currentEditor()) {
    if (editor != nullptr) {
      m_statusBar->setEncoding(editor->encoding());
    }
    else {
      m_statusBar->setEncoding(QString());
    }
  }
}

void TextApplication::convertEols(QAction* action) {
  int new_mode = action->data().toInt();
  TextEditor* current_editor = currentEditor();

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
  m_menuTools->removeAction(m_actionSettings);
  qDeleteAll(m_menuTools->actions());
  m_menuTools->clear();

  m_menuTools->addAction(m_actionSettings);
  m_menuTools->addSeparator();
  m_menuTools->addActions(m_settings->externalTools()->generateActions(m_menuTools, this));
}

void TextApplication::onExternalToolFinished(ExternalTool* tool, QPointer<TextEditor> editor,
                                             const QString& output_text, bool success) {
  if (editor.isNull()) {
    qCritical("Cannot work properly with tool output, assigned text editor was already destroyed, dumping text to output toolbox.");
    m_toolBox->displayOutput(OutputSource::ExternalTool,
                             tr("Cannot deliver output of external tool, assigned text editor no longer exists."),
                             QMessageBox::Icon::Critical);
    return;
  }

  if (!success) {
    m_toolBox->displayOutput(OutputSource::ExternalTool,
                             tr("Tool '%1' reports error: %2.").arg(tool->name(), output_text),
                             QMessageBox::Icon::Critical);
    return;
  }

  switch (tool->output()) {
    case ToolOutput::InsertAtCursorPosition:

      // TODO: zkontrolovat
      editor->insertText(editor->currentPos(), output_text.toUtf8().constData());
      break;

    case ToolOutput::DumpToOutputWindow:
      m_toolBox->displayOutput(OutputSource::ExternalTool, output_text, QMessageBox::Icon::Information);
      break;

    case ToolOutput::NewSavedFile: {
      m_toolBox->displayOutput(OutputSource::ExternalTool,
                               tr("Tool '%1' finished, opening output in new tab.").arg(tool->name()),
                               QMessageBox::Icon::Information);

      loadTextEditorFromFile(IOFactory::writeToTempFile(output_text.toUtf8()), DEFAULT_TEXT_FILE_ENCODING);
      break;
    }

    case ToolOutput::ReplaceSelectionDocument:
      if (!editor->selectionEmpty()) {
        // TODO: ZKONTROLOVAT
        editor->replaceSel(output_text.toUtf8().constData());
      }
      else {
        // We replace whole document contents
        // because there is no selection.
        // NOTE: Using setText() clears history.
        editor->selectAll();

        // TODO: ZKONTROLOVAT
        editor->replaceSel(output_text.toUtf8().constData());
      }

      break;

    case ToolOutput::NoOutput:
    default:
      break;
  }
}

void TextApplication::renameEditor(TextEditor* editor) {
  int index = m_tabEditors->indexOf(editor);

  if (index >= 0) {
    if (!editor->filePath().isEmpty()) {
      m_tabEditors->tabBar()->setTabText(index, QFileInfo(editor->filePath()).fileName());
    }
  }
}
