// For license of this file, see <project-root-folder>/LICENSE.md.

#include "miscellaneous/textapplication.h"

#include "external-tools/externaltool.h"
#include "external-tools/externaltools.h"
#include "gui/dialogs/formfindreplace.h"
#include "gui/dialogs/formmain.h"
#include "gui/messagebox.h"
#include "gui/sidebars/findresultssidebar.h"
#include "gui/sidebars/outputsidebar.h"
#include "gui/statusbar.h"
#include "gui/tabwidget.h"
#include "gui/texteditor.h"
#include "gui/toolbar.h"
#include "miscellaneous/application.h"
#include "miscellaneous/syntaxhighlighting.h"
#include "miscellaneous/textfactory.h"
#include "plugin-system/pluginfactory.h"

#include "3rd-party/uchardet/uchardet.h"

#include <QClipboard>
#include <QDockWidget>
#include <QFileDialog>
#include <QLineEdit>
#include <QPointer>
#include <QTemporaryFile>
#include <QTextCodec>
#include <QTimer>
#include <QWidgetAction>

TextApplication::TextApplication(QObject* parent)
  : QObject(parent), m_shouldSaveSession(false), m_settings(new TextApplicationSettings(this)), m_findReplaceDialog(nullptr) {
  m_outputSidebar = new OutputSidebar(this, nullptr);
  m_actionShowOutputSidebar = m_outputSidebar->generateAction();

  m_findResultsSidebar = new FindResultsSidebar(this, nullptr);
  m_actionShowFindResultsSidebar = m_findResultsSidebar->generateAction();

  // Hook ext. tools early.
  connect(m_settings->externalTools(), &ExternalTools::externalToolsChanged, this, &TextApplication::loadNewExternalTools);
  settings()->pluginFactory()->loadPlugins(this);
}

TextEditor* TextApplication::currentEditor() const {
  return m_tabEditors->textEditorAt(m_tabEditors->currentIndex());
}

void TextApplication::loadTextEditorFromString(const QString& contents) {
  TextEditor* new_editor = new TextEditor(this, m_tabEditors);

  if (m_tabEditors->count() == 1 && !m_tabEditors->textEditorAt(0)->modify() && m_tabEditors->textEditorAt(0)->filePath().isEmpty()) {
    // We have one empty non modified editor already open, close it.
    m_tabEditors->closeTab(0);
  }

  attachTextEditor(new_editor);
  m_tabEditors->setCurrentIndex(addTextEditor(new_editor));
  new_editor->loadFromString(contents);
}

TextEditor* TextApplication::loadTextEditorFromFile(const QString& file_path,
                                                    const QString& explicit_encoding,
                                                    const QString& file_filter) {
  Q_UNUSED(file_filter)

  TextEditor * new_editor = TextEditor::fromTextFile(this, file_path, explicit_encoding);

  if (new_editor != nullptr) {
    if (m_tabEditors->count() == 1 && !m_tabEditors->textEditorAt(0)->modify() &&
        m_tabEditors->textEditorAt(0)->length() == 0 && m_tabEditors->textEditorAt(0)->filePath().isEmpty()) {
      // We have one empty non modified editor already open, close it.
      m_tabEditors->closeTab(0);
    }

    attachTextEditor(new_editor);

    m_settings->setLoadSaveDefaultDirectory(file_path);
    m_tabEditors->setCurrentIndex(addTextEditor(new_editor));

    qobject_cast<QWidget*>(new_editor)->setFocus();
  }

  return new_editor;
}

int TextApplication::addTextEditor(TextEditor* editor) {
  return m_tabEditors->addTab(editor, QIcon(), tr("New text file"), TabType::TextEditor);
}

void TextApplication::attachTextEditor(TextEditor* editor) {
  editor->viewport()->installEventFilter(this);

  connect(editor, &TextEditor::editorReloaded, this, &TextApplication::onEditorReloaded);
  connect(editor, &TextEditor::savedToFile, this, &TextApplication::onEditorSaved);
  connect(editor, &TextEditor::savePointChanged, this, &TextApplication::onSavePointChanged);
  connect(editor, &TextEditor::modified, this, &TextApplication::onEditorModified);
  connect(editor, &TextEditor::requestedVisibility, this, &TextApplication::onEditorRequestedVisibility);
  connect(editor, &TextEditor::readOnlyChanged, this, &TextApplication::onEditorReadOnlyChanged);
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

  foreach (TextEditor* editor, m_tabEditors->editors()) {
    editor->save(&ok);
  }
}

void TextApplication::closeAllUnmodifiedEditors() {
  foreach (TextEditor* editor, m_tabEditors->editors()) {
    if (!editor->modify()) {
      m_tabEditors->closeTab(m_tabEditors->indexOf(editor));
    }
  }
}

void TextApplication::reloadCurrentEditor() {
  TextEditor* editor = currentEditor();

  if (editor != nullptr) {
    editor->reloadFromDisk();
  }
}

void TextApplication::makeEditorVisible(TextEditor* editor) {
  if (editor != nullptr) {
    m_tabEditors->setCurrentWidget(editor);
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
    TextEditor* visible = currentEditor();

    if (visible != nullptr) {
      visible->reloadSettings();
    }
  }
}

void TextApplication::showTabContextMenu(const QPoint& point) {
  QMenu menu;
  const int tab_index = m_tabEditors->tabBar()->tabAt(point);

  if (tab_index >= 0) {
    QAction* act_read_only = menu.addAction(qApp->icons()->fromTheme(QSL("lock")), tr("Read-Only Mode"), [tab_index, this](bool toggle) {
      m_tabEditors->textEditorAt(tab_index)->setReadOnly(toggle);
    });

    act_read_only->setCheckable(true);
    act_read_only->setChecked(m_tabEditors->textEditorAt(tab_index)->readOnly());

    menu.addAction(qApp->icons()->fromTheme(QSL("document-save")), tr("Save"), [tab_index, this]() {
      bool ok;
      m_tabEditors->textEditorAt(tab_index)->save(&ok);
    });
    menu.addAction(qApp->icons()->fromTheme(QSL("window-close")), tr("Close"), [tab_index, this]() {
      m_tabEditors->closeTab(tab_index);
    });
  }
  else {
    menu.addAction(m_actionNoAction);
  }

  menu.exec(m_tabEditors->tabBar()->mapToGlobal(point));
}

void TextApplication::setCurrentEditorAutoIndentEnabled(bool auto_indent_enabled) {
  TextEditor* editor = currentEditor();

  if (editor != nullptr) {
    editor->setAutoIndentEnabled(auto_indent_enabled);
  }

  m_settings->setAutoIndentEnabled(auto_indent_enabled);
}

void TextApplication::setupEolMenu() {
  TextEditor* editor = currentEditor();

  if (editor != nullptr) {
    updateEolMenu(editor->eOLMode());
  }
  else {
    updateEolMenu(settings()->eolMode());
  }
}

void TextApplication::updateEolMenu(int eol_mode) {
  if (m_actionEolMac->actionGroup() == nullptr) {
    QActionGroup* grp_eol = new QActionGroup(m_menuEolMode);

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

void TextApplication::onEditorRequestedVisibility() {
  TextEditor* editor = qobject_cast<TextEditor*>(sender());

  makeEditorVisible(editor);
}

void TextApplication::markEditorModified(TextEditor* editor, bool modified) {
  Q_UNUSED(modified)

  if (m_tabEditors->indexOf(editor) >= 0) {
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

int TextApplication::tabCount() const {
  return m_tabEditors->count();
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
  TextEditor* editor = new TextEditor(this, m_tabEditors);

  attachTextEditor(editor);

  m_tabEditors->setCurrentIndex(addTextEditor(editor));
}

void TextApplication::onEditorModified(int type, int position, int length, int linesAdded, const QByteArray& text,
                                       int line, int foldNow, int foldPrev) {
  Q_UNUSED(position)
  Q_UNUSED(length)
  Q_UNUSED(linesAdded)
  Q_UNUSED(text)
  Q_UNUSED(line)
  Q_UNUSED(foldNow)
  Q_UNUSED(foldPrev)

  if ((type & (SC_MOD_INSERTTEXT | SC_MOD_DELETETEXT)) > 0) {
    TextEditor* editor = qobject_cast<TextEditor*>(sender());

    markEditorModified(editor, editor->modify());
  }
}

void TextApplication::onSavePointChanged() {
  TextEditor* editor = qobject_cast<TextEditor*>(sender());

  renameEditor(editor);
}

void TextApplication::onEditorSaved() {
  TextEditor* editor = qobject_cast<TextEditor*>(sender());

  updateStatusBarFromEditor(editor);
}

void TextApplication::onEditorReloaded() {
  TextEditor* sndr = qobject_cast<TextEditor*>(sender());

  if (sndr == currentEditor()) {
    onEditorTabSwitched(m_tabEditors->indexOf(sndr));
  }
}

void TextApplication::createConnections() {
  // Misc connections.
  connect(qApp, &Application::dataSaveRequested, this, &TextApplication::quit);
  connect(m_settings, &TextApplicationSettings::settingsChanged, this, &TextApplication::reloadEditorsAfterSettingsChanged);

  // Tab widget.
  connect(m_tabEditors, &TabWidget::currentChanged, this, &TextApplication::onEditorTabSwitched);
  connect(m_tabEditors->tabBar(), &TabBar::emptySpaceDoubleClicked, this, &TextApplication::newFile);

  // Actions.
  connect(m_actionPrintCurrentEditor, &QAction::triggered, this, &TextApplication::printCurrentEditor);
  connect(m_actionPrintPreviewCurrentEditor, &QAction::triggered, this, &TextApplication::printPreviewCurrentEditor);
  connect(m_actionFindReplace, &QAction::triggered, this, &TextApplication::showFindReplaceDialog);
  connect(m_actionTabsCloseAllUnmodified, &QAction::triggered, this, &TextApplication::closeAllUnmodifiedEditors);
  connect(m_actionFileSave, &QAction::triggered, this, &TextApplication::saveCurrentEditor);
  connect(m_actionFileSaveAs, &QAction::triggered, this, &TextApplication::saveCurrentEditorAs);
  connect(m_actionFileSaveAll, &QAction::triggered, this, &TextApplication::saveAllEditors);
  connect(m_actionFileReload, &QAction::triggered, this, &TextApplication::reloadCurrentEditor);
  connect(m_actionFileNew, &QAction::triggered, this, &TextApplication::newFile);
  connect(m_actionFileOpen, &QAction::triggered, this, [this]() {
    openTextFile();
  });
  connect(m_actionWordWrap, &QAction::triggered, m_settings, &TextApplicationSettings::setWordWrapEnabled);
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
  m_actionEolUnix = m_mainForm->m_ui.m_actionEolUnix;
  m_actionEolWindows = m_mainForm->m_ui.m_actionEolWindows;
  m_actionEolMac = m_mainForm->m_ui.m_actionEolMac;
  m_actionEolConvertUnix = m_mainForm->m_ui.m_actionEolConvertUnix;
  m_actionEolConvertWindows = m_mainForm->m_ui.m_actionEolConvertWindows;
  m_actionEolConvertMac = m_mainForm->m_ui.m_actionEolConvertMac;
  m_actionFindReplace = m_mainForm->m_ui.m_actionFindReplace;
  m_actionWordWrap = m_mainForm->m_ui.m_actionWordWrap;
  m_actionLineNumbers = m_mainForm->m_ui.m_actionLineNumbers;
  m_actionTabsCloseAllUnmodified = m_mainForm->m_ui.m_actionTabsCloseAllUnmodified;
  m_actionEditBack = m_mainForm->m_ui.m_actionEditBack;
  m_actionEditForward = m_mainForm->m_ui.m_actionEditForward;
  m_actionSettings = m_mainForm->m_ui.m_actionSettings;
  m_actionViewWhitespaces = m_mainForm->m_ui.m_actionViewWhitespaces;
  m_actionViewEols = m_mainForm->m_ui.m_actionViewEols;
  m_actionPrintCurrentEditor = m_mainForm->m_ui.m_actionPrint;
  m_actionPrintPreviewCurrentEditor = m_mainForm->m_ui.m_actionPrintPreview;
  m_actionAutoIndentEnabled = m_mainForm->m_ui.m_actionAutoIndentEnabled;

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

  connect(m_tabEditors->tabBar(), &TabBar::customContextMenuRequested, this, &TextApplication::showTabContextMenu);
  connect(m_mainForm, &FormMain::closeRequested, this, &TextApplication::quit);

  loadState();
  createConnections();
}

QList<QAction*> TextApplication::userActions() const {
  return QList<QAction*>() << m_actionShowFindResultsSidebar << m_actionShowOutputSidebar
                           << settings()->pluginFactory()->assignableActions()
                           << settings()->externalTools()->predefinedToolsActions();
}

void TextApplication::loadState() {
  m_actionWordWrap->setChecked(m_settings->wordWrapEnabled());
  m_actionLineNumbers->setChecked(m_settings->lineNumbersEnabled());
  m_actionViewEols->setChecked(m_settings->viewEols());
  m_actionViewWhitespaces->setChecked(m_settings->viewWhitespaces());

  m_actionEditBack->setEnabled(false);
  m_actionEditForward->setEnabled(false);
  m_settings->externalTools()->reloadTools();

  // Load size/position/visibility of dock widgets.
  m_mainForm->setDockOptions(QMainWindow::DockOption::AnimatedDocks |
                             QMainWindow::DockOption::AllowTabbedDocks | QMainWindow::DockOption::GroupedDragging);
  m_mainForm->setCorner(Qt::Corner::BottomLeftCorner, Qt::DockWidgetArea::LeftDockWidgetArea);
  m_mainForm->setCorner(Qt::Corner::BottomRightCorner, Qt::DockWidgetArea::RightDockWidgetArea);
  m_mainForm->setCorner(Qt::Corner::TopLeftCorner, Qt::DockWidgetArea::LeftDockWidgetArea);
  m_mainForm->setCorner(Qt::Corner::TopRightCorner, Qt::DockWidgetArea::RightDockWidgetArea);

  // We load plugins etc.
  settings()->pluginFactory()->hookPluginsIntoApplication(this);

  // We add built-in sidebars.
  m_menuDockWidgets->addAction(m_actionShowFindResultsSidebar);
  m_menuDockWidgets->addAction(m_actionShowOutputSidebar);

  // We load GUI state of all sidebars.
  QList<BaseSidebar*> sidebars; sidebars << m_outputSidebar << m_findResultsSidebar << settings()->pluginFactory()->sidebars();
  settings()->loadInitialSidebarGuiSettings(m_mainForm, sidebars);
}

void TextApplication::quit(bool* ok) {
  beginSavingSession();

  while (m_tabEditors->count() > 0) {
    if (!m_tabEditors->closeTab(0)) {
      // User aborted.
      *ok = false;
      return;
    }
  }

  *ok = true;

  endSavingSession();
}

bool TextApplication::eventFilter(QObject* obj, QEvent* event) {
  Q_UNUSED(obj)

  if (event->type() == QEvent::Type::Drop) {
    QDropEvent* drop_event = static_cast<QDropEvent*>(event);

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

void TextApplication::printPreviewCurrentEditor() {
  TextEditor* editor = currentEditor();

  if (editor != nullptr) {
    editor->printPreview();
  }
}

void TextApplication::printCurrentEditor() {
  TextEditor* editor = currentEditor();

  if (editor != nullptr) {
    editor->print();
  }
}

void TextApplication::showFindReplaceDialog() {
  if (m_findReplaceDialog == nullptr) {
    m_findReplaceDialog = new FormFindReplace(this, qApp->mainFormWidget());
  }

  m_findReplaceDialog->display();
}

void TextApplication::changeEolMode(QAction* act) {
  TextEditor* editor = currentEditor();
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
  TextEditor* cur_editor = currentEditor();

  if (cur_editor != nullptr) {
    Lexer lexer_act = act->data().value<Lexer>();

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
    QActionGroup* grp = new QActionGroup(m_menuLanguage);

    QMap<QChar, QList<QAction*>> groups;

    foreach (const Lexer& lex, m_settings->syntaxHighlighting()->lexers()) {
      QAction* act = new QAction(QL1S("&") + lex.m_name, m_menuLanguage);

      groups[lex.m_name.at(0)].append(act);

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

  TextEditor* current_editor = currentEditor();

  if (current_editor != nullptr) {
    Lexer lexer = current_editor->lexer();

    foreach (QAction* act, m_lexerActions) {
      Lexer lexer_act = act->data().value<Lexer>();

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
  TextEditor* curr_editor = currentEditor();

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
  TextEditor* editor = currentEditor();

  if (editor != nullptr && !editor->modify()) {
    const QString file_path = editor->filePath();

    if (!file_path.isEmpty() && m_tabEditors->closeTab(m_tabEditors->indexOf(editor))) {
      loadTextEditorFromFile(file_path, action->data().toString());
    }
  }
  else {
    QMessageBox::warning(qApp->mainForm(),
                         tr("Unsaved text file"),
                         tr("Save your file first, please"));
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

void TextApplication::updateToolBarFromEditor(TextEditor* editor, bool only_modified) {
  if (editor != nullptr) {
    if (editor == currentEditor()) {
      // Current editor changed.
      m_actionEditBack->setEnabled(editor->canUndo());
      m_actionEditForward->setEnabled(editor->canRedo());
      m_actionAutoIndentEnabled->setChecked(editor->autoIndentEnabled());

      if (!only_modified) {
        m_actionFileSave->setEnabled(true);
        m_actionFileSaveAs->setEnabled(true);
        m_actionFileReload->setEnabled(true);
        m_menuFileSaveWithEncoding->setEnabled(true);
        m_menuFileReopenWithEncoding->setEnabled(true);
        m_actionFileSaveAll->setEnabled(true);
        m_actionPrintCurrentEditor->setEnabled(true);
        m_actionPrintPreviewCurrentEditor->setEnabled(true);
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
  }
}

void TextApplication::updateStatusBarFromEditor(TextEditor* editor) {
  if (editor == currentEditor()) {
    if (editor != nullptr) {
      m_statusBar->setEol(editor->eOLMode());
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
    const QStringList& session_files = qApp->settings()->value(GROUP(General), SETTING(General::RestoreSessionFiles)).toStringList();
    const QString& user_data_path = qApp->userDataFolder();

    for (const QString& session_file : session_files) {
      if (session_file.startsWith(QL1S("#"))) {
        // Temporary file.
        TextEditor* editor = loadTextEditorFromFile(user_data_path + QDir::separator() + session_file.mid(1));

        if (editor != nullptr) {
          editor->setFilePath(QString());
          m_tabEditors->setTabText(m_tabEditors->indexOf(editor), tr("New text file"));
        }
      }
      else {
        // Real file.
        loadTextEditorFromFile(session_file);
      }
    }
  }

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

void TextApplication::updateEditorIcon(int index, bool modified, bool read_only) {
  if (read_only) {
    m_tabEditors->tabBar()->setTabIcon(index, qApp->icons()->fromTheme(QSL("lock")));
  }
  else {
    m_tabEditors->tabBar()->setTabIcon(index, modified ?
                                       qApp->icons()->fromTheme(QSL("dialog-warning")) :
                                       QIcon());
  }
}

void TextApplication::renameEditor(TextEditor* editor) {
  int index = m_tabEditors->indexOf(editor);

  if (index >= 0) {
    updateEditorIcon(index, editor->modify(), editor->readOnly());

    if (!editor->filePath().isEmpty()) {
      m_tabEditors->tabBar()->setTabText(index, QFileInfo(editor->filePath()).fileName());
      m_tabEditors->tabBar()->setTabToolTip(index, editor->filePath());
    }
  }
}

void TextApplication::onEditorReadOnlyChanged(bool read_only) {
  TextEditor* editor = qobject_cast<TextEditor*>(sender());
  int index = m_tabEditors->indexOf(editor);

  if (index >= 0) {
    updateEditorIcon(index, editor->modify(), read_only);
  }
}

void TextApplication::changeEncoding(QAction* act) {
  TextEditor* editor = currentEditor();

  if (editor != nullptr) {
    editor->setEncoding(act->data().toString().toUtf8());
    updateStatusBarFromEditor(editor);
  }
}
