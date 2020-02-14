// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "saurus/gui/texteditor.h"

#include "common/exceptions/ioexception.h"
#include "common/exceptions/operationcancelledexception.h"
#include "common/gui/messagebox.h"
#include "common/miscellaneous/cryptofactory.h"
#include "common/miscellaneous/iconfactory.h"
#include "common/miscellaneous/iofactory.h"
#include "common/miscellaneous/textfactory.h"
#include "common/network-web/webfactory.h"
#include "definitions/definitions.h"
#include "saurus/gui/sidebars/findresultssidebar.h"
#include "saurus/gui/sidebars/outputsidebar.h"
#include "saurus/gui/texteditorprinter.h"
#include "saurus/miscellaneous/application.h"
#include "saurus/miscellaneous/filemetadata.h"
#include "saurus/miscellaneous/syntaxhighlighting.h"
#include "saurus/miscellaneous/textapplication.h"
#include "saurus/miscellaneous/textapplicationsettings.h"

// *INDENT-OFF*
#include PATH(SCINTILLA_DIR,include/ILoader.h)
#include PATH(SCINTILLA_DIR,include/Platform.h)
#include PATH(SCINTILLA_DIR,include/SciLexer.h)
#include PATH(SCINTILLA_DIR,qt/ScintillaEditBase/PlatQt.h)

// *INDENT-ON*

#include <QDir>
#include <QElapsedTimer>
#include <QFileDialog>
#include <QFileSystemWatcher>
#include <QFontDatabase>
#include <QMenu>
#include <QMouseEvent>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QRegularExpression>
#include <QTextCodec>
#include <QTextStream>
#include <QTimer>

TextEditor::TextEditor(TextApplication* text_app, QWidget* parent)
  : ScintillaEdit(parent), m_saveAgreement(QMessageBox::StandardButton::NoButton), m_isLog(false),
  m_fileWatcher(nullptr), m_settingsDirty(true), m_textApp(text_app),
  m_filePath(QString()), m_encoding(DEFAULT_TEXT_FILE_ENCODING),
  m_lexer(text_app->settings()->syntaxHighlighting()->defaultLexer()),
  m_autoIndentEnabled(text_app->settings()->autoIndentEnabled()),
  m_filePathOnEditorQuit(QString()), m_encryptionPassword(QString()) {

  connect(this, &TextEditor::updateUi, this, &TextEditor::uiUpdated);
  connect(this, &TextEditor::marginClicked, this, &TextEditor::toggleFolding);
  connect(this, &TextEditor::modified, this, &TextEditor::onModified);
  connect(this, &TextEditor::notify, this, &TextEditor::onNotification);
  connect(this, &TextEditor::charAdded, this, &TextEditor::onCharAdded);

  indicSetStyle(INDICATOR_URL, INDIC_COMPOSITIONTHIN);
  indicSetHoverStyle(INDICATOR_URL, INDIC_COMPOSITIONTHICK);
  indicSetAlpha(INDICATOR_FIND, 120);
  indicSetUnder(INDICATOR_FIND, true);
  indicSetStyle(INDICATOR_FIND, INDIC_FULLBOX);
  indicSetHoverStyle(INDICATOR_FIND, INDIC_FULLBOX);

#if defined(Q_OS_WIN)
  setFontQuality(SC_EFF_QUALITY_LCD_OPTIMIZED);
#endif

  //setIndentationGuides(SC_IV_REAL);

  // Set initial settings.
  setMultipleSelection(true);
  setAdditionalSelectionTyping(true);
  setMultiPaste(SC_MULTIPASTE_EACH);
  setVirtualSpaceOptions(SCVS_RECTANGULARSELECTION);
  setCodePage(SC_CP_UTF8);
  setMargins(SC_MAX_MARGIN + 2);
  setMarginWidthN(MARGIN_SYMBOLS, 0);
  setMarginTypeN(MARGIN_LINE_NUMBERS_RIGHT_SPACE, SC_MARGIN_TEXT);
  setWrapVisualFlags(SC_WRAPVISUALFLAG_MARGIN);
  setEndAtLastLine(true);
  setEOLMode(m_textApp->settings()->eolMode());
  setCaretWidth(2);
}

void TextEditor::updateLineNumberMarginWidth(sptr_t zoom, QFont font, sptr_t line_count) {
  // Set point size and add some padding.
  font.setPointSize(font.pointSize() + int(zoom));

  QFontMetrics metr(font);
  int width = TextFactory::stringWidth(QString::number(line_count), metr) + MARGIN_PADDING_LINE_NUMBERS;

  setMarginWidthN(MARGIN_LINE_NUMBERS, qMax(MARGIN_LINE_NUMBERS_MIN_WIDTH + MARGIN_PADDING_LINE_NUMBERS, width));
  setMarginWidthN(MARGIN_LINE_NUMBERS_RIGHT_SPACE, MARGIN_PADDING_LINE_NUMBERS);
}

void TextEditor::loadFromFile(const QByteArray& file_data, const QString& file_path,
                              const QString& encoding, const Lexer& default_lexer, int initial_eol_mode) {
  m_filePath = QDir::toNativeSeparators(file_path);
  m_encoding = encoding.toLocal8Bit();
  m_lexer = default_lexer;

  setEOLMode(initial_eol_mode);

  QTextCodec* codec_for_encoding = QTextCodec::codecForName(m_encoding);

  if (codec_for_encoding == nullptr) {
    qCritical().noquote().nospace() << QSL("We do not have codec for encoding '")
                                    << encoding
                                    << QSL("' when opening file, using defaults.");
    codec_for_encoding = QTextCodec::codecForName(QString(DEFAULT_TEXT_FILE_ENCODING).toLocal8Bit());
    m_encoding = codec_for_encoding->name();
  }

  QTextStream str(file_data); str.setCodec(codec_for_encoding);
  QByteArray str_data = str.readAll().toUtf8();

  blockSignals(true);
  clearAll();
  addText(str_data.size(), str_data);
  emptyUndoBuffer();
  blockSignals(false);
  reattachWatcher(m_filePath);

  emit loadedFromFile(m_filePath);

  // We decide if this file is "log" file.
  setTargetRange(0, lineLength(0));
  setIsLog(searchInTarget(4, ".LOG") != -1);
  setSel(0, 0);
}

void TextEditor::loadFromString(const QString& contents) {
  setText(contents.toUtf8().constData());
  setSel(0, 0);
}

void TextEditor::findAllFromSelectedText() {
  QList<QPair<int, int>> found_ranges;
  int start_position = 0, end_position = int(length()), search_flags = 0;

  while (true) {
    QPair<int, int> found_range = findText(search_flags, getSelText(), start_position, end_position);

    if (found_range.first >= 0) {
      found_ranges.append(found_range);
      start_position = found_range.first == found_range.second ? (found_range.second + 1) : found_range.second;
    }
    else {
      break;
    }
  }

  if (found_ranges.size() <= 1) {
    m_textApp->outputSidebar()->displayOutput(OutputSource::Application,
                                              tr("No other occurrences of \"%1\" found.").arg(QString(getSelText())),
                                              QMessageBox::Icon::Warning);
  }
  else {
    m_textApp->findResultsSidebar()->addResults(this, found_ranges);
  }
}

void TextEditor::uiUpdated(int code) {
  if ((code & (SC_UPDATE_CONTENT | SC_UPDATE_SELECTION | SC_UPDATE_V_SCROLL)) > 0) {
    // Selection has changed.
    // NOTE: We could even allow SC_UPDATE_CONTENT here with QTimer and
    // completely remove uiUpdated method, but it would generate
    // needless useless method calls too often.
    updateOccurrencesHighlights();
  }

  if ((code & (SC_UPDATE_CONTENT | SC_UPDATE_V_SCROLL)) > 0) {
    // Content has changed.
    updateUrlHighlights();
  }
}

void TextEditor::onCharAdded(int chr) {
  if (m_autoIndentEnabled) {
    // We perform auto-indent.
    int target_chr;

    switch (eOLMode()) {
      case SC_EOL_CR:
        target_chr = '\r';
        break;

      default:
        target_chr = '\n';
        break;
    }

    if (chr == target_chr) {
      sptr_t curr_line = lineFromPosition(currentPos());

      if (curr_line > 0) {
        int range_start = int(positionFromLine(curr_line - 1));
        int range_end = int(lineEndPosition(curr_line - 1));

        QPair<int, int> found = findText(SCFIND_REGEXP | SCFIND_CXX11REGEX, "^[ \\t]+", range_start, range_end);

        if (found.first >= 0 && found.second > 0) {
          QByteArray to_insert = textRange(found.first, found.second);

          insertText(currentPos(), to_insert.constData());
          setEmptySelection(currentPos() + to_insert.size());
        }
      }
    }
  }
}

void TextEditor::onNotification(SCNotification* pscn) {
  if (pscn->nmhdr.code == SCN_INDICATORCLICK && pscn->modifiers == SCMOD_CTRL) {
    // Open clicked indicated URL.
    int indic_start = int(indicatorStart(INDICATOR_URL, pscn->position));
    int indic_end = int(indicatorEnd(INDICATOR_URL, pscn->position));

    qApp->web()->openUrlInExternalBrowser(textRange(indic_start, indic_end));
  }
}

void TextEditor::onFileExternallyChanged(const QString& file_path) {
  Q_UNUSED(file_path)

  detachWatcher();

  if (QFile::exists(file_path)) {
    // File is externally changed.
    emit visibilityRequested();
    auto not_again = false;

    // File exists and was externally modified.
    if (m_textApp->settings()->reloadModifiedDocumentsAutomatically() ||
        MessageBox::show(qApp->mainFormWidget(), QMessageBox::Icon::Question, tr("File Externally Modified"),
                         tr("This file was modified outside of %1.").arg(APP_NAME),
                         tr("Do you want to reload file now? This will discard all unsaved changes."),
                         QDir::toNativeSeparators(file_path), QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No,
                         QMessageBox::StandardButton::Yes, &not_again, tr("Reload all files automatically (discard changes)")) ==
        QMessageBox::StandardButton::Yes) {

      reloadFromDisk();

      qApp->showGuiMessage(tr("File '%1' was reloaded from disk.").arg(QDir::toNativeSeparators(file_path)),
                           QMessageBox::Icon::Warning,
                           QUrl::fromLocalFile(file_path), this);

      if (not_again) {
        m_textApp->settings()->setReloadModifiedDocumentsAutomatically(true);
      }
    }
  }
}

void TextEditor::onModified(int type, int position, int length, int lines_added, const QByteArray& text,
                            int line, int fold_now, int fold_prev) {
  Q_UNUSED(position)
  Q_UNUSED(length)
  Q_UNUSED(type)
  Q_UNUSED(text)
  Q_UNUSED(line)
  Q_UNUSED(fold_now)
  Q_UNUSED(fold_prev)

  if (lines_added != 0) {
    updateLineNumberMarginVisibility();
  }
}

void TextEditor::contextMenuEvent(QContextMenuEvent* event) {
  QMenu context_menu;

  context_menu.addAction(qApp->icons()->fromTheme(QSL("edit-find")), tr("&Find All"), this,
                         &TextEditor::findAllFromSelectedText)->setEnabled(!selectionEmpty());
  context_menu.addSeparator();
  context_menu.addAction(m_textApp->m_actionEditBack);
  context_menu.addAction(m_textApp->m_actionEditForward);
  context_menu.addSeparator();
  context_menu.addAction(qApp->icons()->fromTheme(QSL("edit-select-all")), tr("&Select All"), [this]() {
    selectAll();
  })->setEnabled(length() > 0);
  context_menu.addAction(qApp->icons()->fromTheme(QSL("edit-cut")), tr("&Cut"), [this]() {
    cut();
  })->setEnabled(!selectionEmpty());
  context_menu.addAction(qApp->icons()->fromTheme(QSL("edit-copy")), tr("&Copy"), [this]() {
    copy();
  })->setEnabled(!selectionEmpty());
  context_menu.addAction(qApp->icons()->fromTheme(QSL("edit-paste")), tr("&Paste"), [this]() {
    paste();
  })->setEnabled(canPaste());

  context_menu.exec(event->globalPos());
}

void TextEditor::wheelEvent(QWheelEvent* event) {
  if (event->orientation() == Qt::Horizontal) {
    if (horizontalScrollBarPolicy() == Qt::ScrollBarAlwaysOff) {
      event->ignore();
    }
    else {
      // NOTE: Deliberately skipping ScintillaEdit implementation.
      QAbstractScrollArea::wheelEvent(event);
    }
  }
  else {
    if ((QGuiApplication::keyboardModifiers() & Qt::KeyboardModifier::ControlModifier) == Qt::KeyboardModifier::ControlModifier) {
      if (event->delta() > 0) {
        m_textApp->settings()->increaseFontSize();
      }
      else {
        m_textApp->settings()->decreaseFontSize();
      }
    }
    else if ((QGuiApplication::keyboardModifiers() & Qt::KeyboardModifier::ShiftModifier) == Qt::KeyboardModifier::ShiftModifier) {
      if (event->delta() > 0) {
        m_textApp->settings()->increaseLineSpacing();
      }
      else {
        m_textApp->settings()->decreaseLineSpacing();
      }
    }
    else {
      if (verticalScrollBarPolicy() == Qt::ScrollBarPolicy::ScrollBarAlwaysOff) {
        event->ignore();
      }
      else {
        // NOTE: Deliberately skipping ScintillaEdit implementation.
        QAbstractScrollArea::wheelEvent(event);
      }
    }
  }
}

QString TextEditor::requestSaveFileName(QString* selected_filter) const {
  return MessageBox::getSaveFileName(qApp->mainFormWidget(),
                                     tr("Save File as"),
                                     m_filePath.isEmpty()
                                     ? m_textApp->settings()->loadSaveDefaultDirectory()
                                     : m_filePath,
                                     m_textApp->settings()->syntaxHighlighting()->fileFilters(),
                                     selected_filter);
}

void TextEditor::appendSessionFile(const QString& file_name, bool is_nonexistent) {
  QString file_n = is_nonexistent ? (QL1S("#") + file_name) : file_name;

  qApp->settings()->setValue(GROUP(General),
                             General::RestoreSessionFiles,
                             qApp->settings()->value(GROUP(General),
                                                     SETTING(General::RestoreSessionFiles)).toStringList() << file_n);
}

QString TextEditor::getSessionFile() {
  QDir dir_data(qApp->userDataFolder());
  QString file_name;
  int counter = 1;

  do {
    file_name = QString("tab_%2_%1.session").arg(OS_ID_LOW, QString::number(counter++));
  } while (dir_data.exists(file_name));

  return file_name;
}

QString TextEditor::encryptionPassword() const {
  return m_encryptionPassword;
}

void TextEditor::setEncryptionPassword(const QString& encryption_password) {
  m_encryptionPassword = encryption_password;
}

QMessageBox::StandardButton TextEditor::currentSaveAgreement() const {
  return m_saveAgreement;
}

void TextEditor::resetSaveAgreement() {
  m_saveAgreement = QMessageBox::StandardButton::NoButton;
  m_filePathOnEditorQuit.clear();
}

void TextEditor::askForSaveAgreement() {
  // We determine if there is any dialog to be shown,
  // when this text editor is about to be closed.
  if (!(m_textApp->shouldSaveSession() && filePath().isEmpty()) &&
      !(m_textApp->shouldSaveSession() && !filePath().isEmpty() && QFile::exists(filePath()) && !modify()) &&
      (modify() || (!filePath().isEmpty() && !QFile::exists(filePath())))) {
    emit requestVisibility();

    m_saveAgreement = MessageBox::show(qApp->mainFormWidget(),
                                       QMessageBox::Icon::Question,
                                       tr("Unsaved Changes"),
                                       tr("This document has unsaved changes, do you want to save them?"),
                                       QString(),
                                       filePath(),
                                       QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
                                       QMessageBox::Save);

    if (m_saveAgreement == QMessageBox::StandardButton::Save) {
      if (m_filePath.isEmpty()) {
        m_filePathOnEditorQuit = requestSaveFileName();

        if (m_filePathOnEditorQuit.isEmpty()) {
          m_saveAgreement = QMessageBox::StandardButton::Cancel;
        }
      }
    }
    else {
      m_filePathOnEditorQuit.clear();
    }
  }
  else {
    // This editor does not need save agreement, no "save/discard"
    // dialog is needed.
    m_saveAgreement = QMessageBox::StandardButton::Save;
  }
}

void TextEditor::setFocus() {
  qobject_cast<QWidget*>(this)->setFocus();
}

void TextEditor::requestVisibility() {
  emit visibilityRequested();
}

void TextEditor::setFilePath(const QString& file_path) {
  m_filePath = file_path;
}

void TextEditor::updateUrlHighlights() {
  // FIXME: This method is VERY slow as it can run even several seconds
  // when opened file has long rows.
  QElapsedTimer tmr;

  tmr.start();

  setIndicatorCurrent(INDICATOR_URL);
  indicatorClearRange(0, length());

  // Count of lines visible on screen.
  sptr_t visible_lines_count = linesOnScreen();
  sptr_t first_visible_position = positionFromPoint(1, 1);
  int start_position = int(first_visible_position);

  // First line visible on screen.
  sptr_t first_visible_line = lineFromPosition(start_position);
  sptr_t ideal_end_position = positionFromLine(first_visible_line + visible_lines_count) +
                              lineLength(first_visible_line + visible_lines_count);
  int end_position = ideal_end_position < 0 ? int(length()) : int(ideal_end_position);
  int search_flags = SCFIND_CXX11REGEX | SCFIND_REGEXP;

  while (true) {
    QPair<int, int> found_range = findText(search_flags,
                                           "((((https?|ftp|mailto):(\\/\\/)?)|(www\\.))[\\w\\-.~:\\/?#@%!$&'*+,;=`.]+)|"
                                           "([\\w\\-.~:\\/?#@!$&*+,;=`.]+@[\\w\\-\\.]+)",
                                           start_position,
                                           end_position);

    if (found_range.first >= 0) {
      indicatorFillRange(found_range.first, found_range.second - found_range.first);
      start_position = found_range.first == found_range.second ? (found_range.second + 1) : found_range.second;
    }
    else {
      break;
    }
  }

  qDebugNN << QSL("URL highlighting took ") << tmr.elapsed() << QSL(" miliseconds");
}

void TextEditor::updateOccurrencesHighlights() {
  QByteArray sel_text = getSelText();

  // Count of lines visible on screen.
  sptr_t visible_lines_count = linesOnScreen();
  int first_visible_position = int(positionFromPoint(1, 1));

  // Firs line visible on screen.
  sptr_t first_visible_line = lineFromPosition(first_visible_position);
  sptr_t ideal_end_position = positionFromLine(first_visible_line + visible_lines_count) +
                              lineLength(first_visible_line + visible_lines_count);
  int end_position = ideal_end_position < 0 ? int(length()) : int(ideal_end_position);

  setIndicatorCurrent(INDICATOR_FIND);
  indicatorClearRange(first_visible_position, end_position - first_visible_position);

#if defined(DEBUG)
  qDebug().noquote() << QSL("Occurrences highlights updated.");
#endif

  if (!sel_text.isEmpty()) {
    bool context_aware = m_textApp->settings()->contextAwareHighlightingEnabled();
    int search_flags = context_aware ? SCFIND_WHOLEWORD : 0;

    while (true) {
      QPair<int, int> found_range = findText(search_flags, sel_text.constData(), first_visible_position, end_position);

      if (found_range.first < 0) {
        break;
      }
      else {
        if (found_range.first != qMin(selectionStart(), selectionEnd()) &&
            found_range.second != qMax(selectionStart(), selectionEnd())) {
          indicatorFillRange(found_range.first, found_range.second - found_range.first);
        }

        first_visible_position = found_range.first == found_range.second ? (found_range.second + 1) : found_range.second;
      }
    }
  }
}

void TextEditor::detachWatcher() {
  if (m_fileWatcher != nullptr) {
    if (!m_fileWatcher->files().isEmpty()) {
      m_fileWatcher->removePaths(m_fileWatcher->files());
    }
  }
}

void TextEditor::reattachWatcher(const QString& file_path) {
  if (m_fileWatcher == nullptr) {
    m_fileWatcher = new QFileSystemWatcher(this);

    connect(m_fileWatcher, &QFileSystemWatcher::fileChanged, this, &TextEditor::onFileExternallyChanged);
  }

  if (!m_fileWatcher->files().isEmpty()) {
    m_fileWatcher->removePaths(m_fileWatcher->files());
  }

  if (!file_path.isEmpty()) {
    m_fileWatcher->addPath(file_path);
  }
}

bool TextEditor::isMarginVisible(int margin_number) const {
  return marginWidthN(margin_number) > 0;
}

void TextEditor::reloadFont() {
  QFont new_font = m_textApp->settings()->mainFont();

  if (styleFont(STYLE_DEFAULT) != new_font.family().toUtf8() || styleSize(STYLE_DEFAULT) != new_font.pointSize()) {
    styleSetFont(STYLE_DEFAULT, new_font.family().toUtf8().constData());
    styleSetSize(STYLE_DEFAULT, new_font.pointSize());
  }

  // Copy all font properties to all other styles.
  // WARNING: All styles/highlighting stuff must be reloaded after reloading fonts too.
  styleClearAll();
  updateLineNumberMarginVisibility();
}

void TextEditor::reloadSettings() {
  if (m_settingsDirty) {
    int line_spacing = m_textApp->settings()->lineSpacing();

    setIndent(m_textApp->settings()->indentSize());
    setTabWidth(m_textApp->settings()->tabSize());
    setUseTabs(m_textApp->settings()->indentWithTabs());

    setExtraAscent(line_spacing / 2);
    setExtraDescent(line_spacing / 2);

    setWrapMode(m_textApp->settings()->wordWrapEnabled() ? SC_WRAP_WORD : SC_WRAP_NONE);
    setViewEOL(m_textApp->settings()->viewEols());
    setViewWS(m_textApp->settings()->viewWhitespaces() ? SCWS_VISIBLEALWAYS : SCWS_INVISIBLE);

    reloadFont();
    reloadLexer(m_lexer);

    // NOTE: Give text editor time to properly redraw itself.
    QTimer::singleShot(500, this, &TextEditor::updateOccurrencesHighlights);
    QTimer::singleShot(500, this, &TextEditor::updateUrlHighlights);

    m_settingsDirty = false;
  }
}

void TextEditor::reloadLexer(const Lexer& default_lexer) {
  m_lexer = default_lexer;
  setLexer(m_lexer.m_code);

  auto color_theme = m_textApp->settings()->syntaxHighlighting()->currentColorTheme();

  // Setup indicators to use colors from current theme (if defined).
  indicSetFore(INDICATOR_URL, color_theme.hasComponent(SyntaxColorTheme::StyleComponents::ScintillaUrlHighlight) ?
               QCOLOR_TO_SPRT(color_theme.component(SyntaxColorTheme::StyleComponents::ScintillaUrlHighlight).m_colorForeground) :
               RGB_TO_SPRT(0, 225, 0));
  indicSetFore(INDICATOR_FIND, color_theme.hasComponent(SyntaxColorTheme::StyleComponents::ScintillaOccurrencesHighlight) ?
               QCOLOR_TO_SPRT(color_theme.component(SyntaxColorTheme::StyleComponents::ScintillaOccurrencesHighlight).m_colorForeground) :
               RGB_TO_SPRT(220, 30, 0));

  // Style with number 0 always black.
  //styleSetFore(0, 0);

  // Gray whitespace characters.
  setWhitespaceFore(true, QCOLOR_TO_SPRT(color_theme.component(SyntaxColorTheme::StyleComponents::ScintillaControlChar).m_colorForeground));
  setWhitespaceSize(3);

  // We set special style classes here.
  color_theme.component(SyntaxColorTheme::StyleComponents::Default).applyToEditor(*this, STYLE_DEFAULT);

  // Override background color.
  styleSetBack(STYLE_DEFAULT, QCOLOR_TO_SPRT(color_theme.component(SyntaxColorTheme::StyleComponents::ScintillaPaper).m_colorBackground));

  // All "STYLE_DEFAULT" properties now get copied into other style classes.
  styleClearAll();

  color_theme.component(SyntaxColorTheme::StyleComponents::ScintillaMargin).applyToEditor(*this, STYLE_LINENUMBER);
  color_theme.component(SyntaxColorTheme::StyleComponents::ScintillaControlChar).applyToEditor(*this, STYLE_CONTROLCHAR);

  // Set selection colors.
  setSelFore(true, QCOLOR_TO_SPRT(color_theme.component(SyntaxColorTheme::StyleComponents::ScintillaPaper).m_colorBackground));
  setSelBack(true, QCOLOR_TO_SPRT(color_theme.component(SyntaxColorTheme::StyleComponents::Default).m_colorForeground));

  // Set caret colors.
  setCaretFore(QCOLOR_TO_SPRT(color_theme.component(SyntaxColorTheme::StyleComponents::Default).m_colorForeground));

  if (color_theme.component(SyntaxColorTheme::StyleComponents::ScintillaCurrentLine).m_colorBackground.isValid()) {
    setCaretLineVisible(true);
    setCaretLineBack(QCOLOR_TO_SPRT(color_theme.component(SyntaxColorTheme::StyleComponents::ScintillaCurrentLine).m_colorBackground));
  }
  else {
    setCaretLineVisible(false);
  }

  // Setup edge mode.
  if (m_textApp->settings()->edgeLineEnabled()) {
    setEdgeColumn(m_textApp->settings()->edgeLineColumn());
    setEdgeMode(EDGE_LINE);

    // We set color of the edge line and fallback to "red" if current
    // color scheme does not define one.
    setEdgeColour(color_theme.hasComponent(SyntaxColorTheme::StyleComponents::ScintillaEdge) ?
                  QCOLOR_TO_SPRT(color_theme.component(SyntaxColorTheme::StyleComponents::ScintillaEdge).m_colorForeground) :
                  QCOLOR_TO_SPRT(QColor(Qt::GlobalColor::red)));
  }
  else {
    setEdgeMode(EDGE_NONE);
  }

  if (m_lexer.m_code != SCLEX_NULL) {
    // Load more specific colors = keywords, operators etc.
    for (int i = 0; i <= STYLE_MAX; i++) {
      // We set colors for all non-predefined styles.
      if (i < STYLE_DEFAULT || i > STYLE_LASTPREDEFINED) {
        if (default_lexer.m_styleMappings.contains(i)) {
          // Lexer specifies mapping of Scintilla style code into Textosaurus style code, we use it.

          auto color_component = color_theme.component(default_lexer.m_styleMappings.value(i));

          color_component.applyToEditor(*this, i);
        }
        else if (default_lexer.m_styleMappings.contains(0)) {
          // We use "default" style color.
          auto color_component = color_theme.component(default_lexer.m_styleMappings.value(0));

          color_component.applyToEditor(*this, i);
        }
        else {
          // Lexer does not define mapping to neither specific style nor default style,
          // use randomized style.
          auto color_component = color_theme.randomizedComponent(i);

          color_component.applyToEditor(*this, i);
        }
      }
    }
  }

  // Setup keywords.
  QMapIterator<int, QString> i(m_lexer.m_keywords);

  while (i.hasNext()) {
    i.next();
    setKeyWords( i.key(), i.value().toLocal8Bit().constData());
  }

  qDebug().noquote() << QSL("Current lexer offers these properties:") << propertyNames();

  if (m_textApp->settings()->codeFoldingEnabled() &&
      m_lexer.m_code != SCLEX_NULL &&
      m_lexer.m_code != SCLEX_CONTAINER &&

      // NOTE: Added hardcoded folding support for old-style lexers.
      (propertyNames().contains("fold") || m_lexer.m_supportsOldStyleFolding)) {
    // We activate folding.
    setProperty("fold", "1");
    setProperty("fold.compact", "1");
    setProperty("fold.html", "1");
    setMarginWidthN(MARGIN_FOLDING, MARGIN_WIDTH_FOLDING);

    setFoldFlags(SC_FOLDFLAG_LINEAFTER_CONTRACTED | SC_FOLDFLAG_LINEBEFORE_CONTRACTED);
    setMarginSensitiveN(MARGIN_FOLDING, true);
    setMarginMaskN(MARGIN_FOLDING, SC_MASK_FOLDERS);

    markerDefine(SC_MARKNUM_FOLDER, SC_MARK_PLUS);
    markerDefine(SC_MARKNUM_FOLDEROPEN, SC_MARK_MINUS);
    markerDefine(SC_MARKNUM_FOLDEREND, SC_MARK_EMPTY);
    markerDefine(SC_MARKNUM_FOLDEROPENMID, SC_MARK_EMPTY);
    markerDefine(SC_MARKNUM_FOLDERSUB, SC_MARK_VLINE);
    markerDefine(SC_MARKNUM_FOLDERMIDTAIL, SC_MARK_TCORNER);
    markerDefine(SC_MARKNUM_FOLDERTAIL, SC_MARK_LCORNER);

    auto default_thm_comp = color_theme.component(SyntaxColorTheme::StyleComponents::Default);
    auto paper_thm_comp = color_theme.component(SyntaxColorTheme::StyleComponents::ScintillaPaper);

    // Frame and inner part.
    markerSetFore(SC_MARKNUM_FOLDER, QCOLOR_TO_SPRT(default_thm_comp.m_colorForeground));
    markerSetBack(SC_MARKNUM_FOLDER, QCOLOR_TO_SPRT(paper_thm_comp.m_colorBackground));
    markerSetFore(SC_MARKNUM_FOLDEROPEN, QCOLOR_TO_SPRT(default_thm_comp.m_colorForeground));
    markerSetBack(SC_MARKNUM_FOLDEROPEN, QCOLOR_TO_SPRT(paper_thm_comp.m_colorBackground));

    // Lines.
    markerSetBack(SC_MARKNUM_FOLDERSUB, QCOLOR_TO_SPRT(default_thm_comp.m_colorForeground));
    markerSetBack(SC_MARKNUM_FOLDERMIDTAIL, QCOLOR_TO_SPRT(default_thm_comp.m_colorForeground));
    markerSetBack(SC_MARKNUM_FOLDERTAIL, QCOLOR_TO_SPRT(default_thm_comp.m_colorForeground));

    if (color_theme.component(SyntaxColorTheme::StyleComponents::ScintillaCurrentLine).m_colorBackground.isValid()) {
      setFoldMarginHiColour(true,
                            QCOLOR_TO_SPRT(color_theme.component(SyntaxColorTheme::StyleComponents::ScintillaCurrentLine).
                                           m_colorBackground));
    }
    else {
      setFoldMarginHiColour(true,
                            QCOLOR_TO_SPRT(color_theme.component(SyntaxColorTheme::StyleComponents::ScintillaMargin).m_colorBackground));
    }

    setFoldMarginColour(true,
                        QCOLOR_TO_SPRT(color_theme.component(SyntaxColorTheme::StyleComponents::ScintillaMargin).m_colorBackground));
  }
  else {
    setProperty("fold", "0");
    setProperty("fold.compact", "0");
    setProperty("fold.html", "0");
    setMarginWidthN(MARGIN_FOLDING, 0);

    // Make sure everything is expanded.
    foldAll(SC_FOLDACTION_EXPAND);
  }

  colourise(0, -1);
}

void TextEditor::saveToFile(const QString& file_path, bool& ok, const QString& encoding) {
  QFile file(file_path);

  detachWatcher();

  QDir().mkpath(QFileInfo(file_path).absolutePath());

  if (!file.open(QIODevice::Truncate | QIODevice::WriteOnly)) {
    MessageBox::show(qApp->mainFormWidget(),
                     QMessageBox::Icon::Critical,
                     tr("Cannot Save to File"),
                     tr("Document cannot be saved to file because the destination is probably non-writable."));
    ok = false;
    reattachWatcher(m_filePath);
    return;
  }

  if (!encoding.isEmpty()) {
    m_encoding = encoding.toLocal8Bit();
  }

  if (!m_encryptionPassword.isEmpty()) {
    QByteArray utf_text = getText(length() + 1);
    QByteArray converted_text;
    QTextStream str(&converted_text);

    // We convert text to target encoding.
    str.setCodec(m_encoding.constData());
    str << QString::fromUtf8(utf_text);
    str.flush();

    // We encrypt text and save it.
    try {
      QByteArray encrypted = CryptoFactory::encryptData(m_encryptionPassword, converted_text);

      file.write(encrypted);
      file.flush();
    }
    catch (const ApplicationException& ex) {
      MessageBox::show(qApp->mainFormWidget(),
                       QMessageBox::Icon::Critical,
                       tr("Cannot Save to File"),
                       tr("Encryption of file failed: %1.").arg(ex.message()));

      file.close();
      ok = false;
      reattachWatcher(m_filePath);
      return;
    }
  }
  else {
    QTextStream str(&file);
    QByteArray text = getText(length() + 1);

    str.setCodec(m_encoding.constData());
    str << text;
    str.flush();
  }

  file.close();
  m_filePath = QDir::toNativeSeparators(file_path);
  reattachWatcher(m_filePath);

  setSavePoint();
  emit savedToFile(m_filePath);

  ok = true;
}

bool TextEditor::autoIndentEnabled() const {
  return m_autoIndentEnabled;
}

void TextEditor::setAutoIndentEnabled(bool auto_indent_enabled) {
  m_autoIndentEnabled = auto_indent_enabled;
}

bool TextEditor::isLog() const {
  return m_isLog;
}

void TextEditor::setIsLog(bool is_log) {
  m_isLog = is_log;

  if (m_isLog) {
    gotoPos(length());
    newLine();

    const QString date_str = QDateTime::currentDateTime().toString(m_textApp->settings()->logTimestampFormat());

    insertText(currentPos(), date_str.toUtf8().constData());
    gotoPos(length());
    newLine();
    setSavePoint();
  }
}

bool TextEditor::settingsDirty() const {
  return m_settingsDirty;
}

void TextEditor::setSettingsDirty(bool settings_dirty) {
  m_settingsDirty = settings_dirty;
}

void TextEditor::setReadOnly(bool read_only) {
  if (read_only != readOnly()) {
    ScintillaEdit::setReadOnly(read_only);
    emit readOnlyChanged(read_only);
  }
}

TextEditor* TextEditor::fromTextFile(TextApplication* app, const QString& file_path,
                                     const QString& explicit_encoding, const QString& explicit_filter) {
  try {

    QFileInfo nfo(file_path);
    QString file_target_path = file_path;

    if (nfo.isSymLink() && !nfo.symLinkTarget().isEmpty()) {
      qWarningNN << QSL("File '") << file_path << QSL("' is symlink with target at '") << nfo.symLinkTarget() << QSL("'.");
      file_target_path = nfo.symLinkTarget();
    }

    auto file_data = FileMetadata::obtainRawFileData(file_target_path);
    FileMetadata metadata = FileMetadata::getInitialMetadata(file_data.first, file_target_path, explicit_encoding, explicit_filter);

    if (!metadata.m_encoding.isEmpty()) {
      auto* new_editor = new TextEditor(app, qApp->mainFormWidget());

      new_editor->loadFromFile(file_data.first, file_target_path, metadata.m_encoding, metadata.m_lexer, metadata.m_eolMode);
      new_editor->setEncryptionPassword(file_data.second);
      return new_editor;
    }
    else {
      return nullptr;
    }
  }
  catch (const OperationCancelledException&) {
    qDebugNN << QSL("User cancelled decryption password prompt.");
    return nullptr;
  }
  catch (const ApplicationException& ex) {
    QMessageBox::critical(qApp->mainFormWidget(), QObject::tr("Cannot read file"),
                          QObject::tr("File '%1' cannot be opened for reading, reason: %2.").arg(QDir::toNativeSeparators(file_path),
                                                                                                 ex.message()));
    return nullptr;
  }
}

void TextEditor::reloadFromDisk() {
  if (!filePath().isEmpty()) {
    if (modify()) {
      QMessageBox::StandardButton answer = MessageBox::show(qApp->mainFormWidget(),
                                                            QMessageBox::Icon::Question,
                                                            tr("Unsaved Changes"),
                                                            tr("This document has unsaved changes, "
                                                               "do you want to ignore the changes and reload file?"),
                                                            QString(),
                                                            !filePath().isEmpty() ? filePath() : QString(),
                                                            QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No,
                                                            QMessageBox::StandardButton::Yes);

      if (answer != QMessageBox::StandardButton::Yes) {
        return;
      }
    }

    // We reload now.
    // When we reload, we automatically detect EOL and encoding
    // and show no warnings, makes no sense in this use-case.
    try {
      auto file_data = FileMetadata::obtainRawFileData(filePath());
      FileMetadata metadata = FileMetadata::getInitialMetadata(file_data.first, filePath());
      auto current_line = lineFromPosition(currentPos());

      if (!metadata.m_encoding.isEmpty()) {
        loadFromFile(file_data.first, filePath(), metadata.m_encoding, metadata.m_lexer, metadata.m_eolMode);
        setEncryptionPassword(file_data.second);
        setSettingsDirty(true);

        emit editorReloaded();
        emit savePointChanged(false);

        if (IS_IN_ARRAY(current_line, lineCount())) {
          auto position_new_document = positionFromLine(current_line);

          setSel(position_new_document, position_new_document);
        }
      }
    }
    catch (const OperationCancelledException&) {
      qDebug().noquote() << QSL("User cancelled decryption password prompt.");
    }
    catch (const ApplicationException& ex) {
      QMessageBox::critical(qApp->mainFormWidget(), QObject::tr("Cannot read file"),
                            QObject::tr("File '%1' cannot be opened for reading, reason: %2.").arg(QDir::toNativeSeparators(filePath()),
                                                                                                   ex.message()));
    }
  }
}

void TextEditor::setEncoding(const QByteArray& encoding) {
  m_encoding = encoding;
}

void TextEditor::updateLineNumberMarginVisibility() {
  const int current_width = int(marginWidthN(MARGIN_LINE_NUMBERS));
  const bool should_be_visible = m_textApp->settings()->lineNumbersEnabled();

  if (current_width <= 0 && !should_be_visible) {
    // We do not have to make anything.
    return;
  }

  if (should_be_visible) {
    updateLineNumberMarginWidth(zoom(), m_textApp->settings()->mainFont(), lineCount());
  }
  else {
    setMarginWidthN(MARGIN_LINE_NUMBERS, 0);
    setMarginWidthN(MARGIN_LINE_NUMBERS_RIGHT_SPACE, 0);
  }
}

void TextEditor::toggleFolding(int position, int modifiers, int margin) {
  Q_UNUSED(modifiers)

  const sptr_t line_number = lineFromPosition(position);

  switch (margin) {
    case MARGIN_FOLDING:
      toggleFold(line_number);
      break;

    default:
      break;
  }
}

void TextEditor::printPreview(bool black_on_white) {
  TextEditorPrinter printer;

  if (black_on_white) {
    setPrintColourMode(SC_PRINT_BLACKONWHITE);
  }
  else {
    setPrintColourMode(SC_PRINT_NORMAL);
  }

  printer.setZoom(-2);

  QPrintPreviewDialog dialog(&printer, qApp->mainFormWidget());

  connect(&dialog, &QPrintPreviewDialog::paintRequested, this, [this](QPrinter* prntr) {
    auto* sndr = dynamic_cast<TextEditorPrinter*>(prntr);

    if (sndr != nullptr) {
      sndr->printRange(this);
    }
  });

  dialog.exec();
}

void TextEditor::print(bool black_on_white) {
  TextEditorPrinter printer;

  if (black_on_white) {
    setPrintColourMode(SC_PRINT_BLACKONWHITE);
  }
  else {
    setPrintColourMode(SC_PRINT_NORMAL);
  }

  printer.setZoom(-2);

  QPrintDialog dialog(&printer, qApp->mainFormWidget());

  if (dialog.exec() == QDialog::DialogCode::Accepted) {
    printer.printRange(this);
  }
}

Lexer TextEditor::lexer() const {
  return m_lexer;
}

QByteArray TextEditor::encoding() const {
  return m_encoding;
}

void TextEditor::save(bool& ok) {
  if (m_filePath.isEmpty()) {
    // Newly created document, save as.
    saveAs(ok);
  }
  else {
    // We just save this modified document to same file.
    saveToFile(m_filePath, ok);
  }
}

void TextEditor::saveAs(bool& ok, const QString& encoding) {
  // We save this documents as new file.
  QString file_path;
  QString sel_filter;

  if (m_filePathOnEditorQuit.isEmpty()) {
    file_path = requestSaveFileName(&sel_filter);
  }
  else {
    file_path = m_filePathOnEditorQuit;
  }

  if (!file_path.isEmpty()) {
    //m_textApp->settings()->setLoadSaveDefaultDirectory(file_path);

    // Reload lexer if needed.
    if (!sel_filter.isEmpty()) {
      try {
        Lexer new_lexer = qApp->textApplication()->settings()->syntaxHighlighting()->lexerForFilter(sel_filter);

        if (lexer().m_code != new_lexer.m_code) {
          m_lexer = new_lexer;
          setSettingsDirty(true);
        }
      }
      catch (ApplicationException& ex) {
        qWarningNN << QSL("Failed to find syntax highlighter: %1").arg(ex.message());
      }
    }

    if (encoding.isEmpty()) {
      saveToFile(file_path, ok);
    }
    else {
      saveToFile(file_path, ok, encoding);
    }
  }
  else {
    ok = false;
  }
}

void TextEditor::closeEditor(bool& ok) {
  if (m_textApp->shouldSaveSession() && filePath().isEmpty()) {
    // Store even empty editors to session.W
    // We save this editor "into" temporary session file.
    QString session_file = getSessionFile();

    saveToFile(qApp->userDataFolder() + QDir::separator() + session_file, ok, DEFAULT_TEXT_FILE_ENCODING);

    // File is saved, we store the filename.
    if (ok) {
      appendSessionFile(session_file, true);
    }
  }
  else if (m_textApp->shouldSaveSession() && !filePath().isEmpty() && QFile::exists(filePath()) && !modify()) {
    // No need to save, just mark to session if needed.
    appendSessionFile(QDir::toNativeSeparators(filePath()), false);
    ok = true;
  }
  else if (modify() || (!filePath().isEmpty() && !QFile::exists(filePath()))) {
    emit visibilityRequested();

    if (currentSaveAgreement() == QMessageBox::StandardButton::NoButton) {
      // We have no agreement, request it.
      askForSaveAgreement();
    }

    // We need to save.
    QMessageBox::StandardButton response = currentSaveAgreement();

    switch (response) {
      case QMessageBox::StandardButton::Save: {
        bool ok_save = false;

        save(ok_save);
        ok = ok_save;

        if (ok_save && m_textApp->shouldSaveSession()) {
          appendSessionFile(QDir::toNativeSeparators(filePath()), false);
        }

        break;
      }

      case QMessageBox::StandardButton::Discard:
        ok = true;

        if (m_textApp->shouldSaveSession()) {
          appendSessionFile(QDir::toNativeSeparators(filePath()), false);
        }

        break;

      case QMessageBox::StandardButton::Cancel:
        ok = false;
        break;

      default:
        ok = false;
        break;
    }

    resetSaveAgreement();
  }
  else {
    ok = true;
  }
}

QString TextEditor::filePath() const {
  return m_filePath;
}
