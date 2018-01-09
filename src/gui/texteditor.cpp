// For license of this file, see <project-root-folder>/LICENSE.md.

#include "gui/texteditor.h"

#include "definitions/definitions.h"
#include "exceptions/ioexception.h"
#include "gui/messagebox.h"
#include "gui/sidebars/outputsidebar.h"
#include "gui/texteditorprinter.h"
#include "miscellaneous/application.h"
#include "miscellaneous/iofactory.h"
#include "miscellaneous/syntaxhighlighting.h"
#include "miscellaneous/textapplication.h"
#include "miscellaneous/textapplicationsettings.h"
#include "miscellaneous/textfactory.h"
#include "network-web/webfactory.h"

#include "scintilla/include/ILoader.h"
#include "scintilla/include/Platform.h"
#include "scintilla/include/SciLexer.h"
#include "scintilla/qt/ScintillaEditBase/PlatQt.h"

#include <QDir>
#include <QFileDialog>
#include <QFileSystemWatcher>
#include <QFontDatabase>
#include <QMouseEvent>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QRegularExpression>
#include <QTextCodec>
#include <QTextStream>

#include "hoedown/hdocument.h"
#include "hoedown/html.h"

TextEditor::TextEditor(TextApplication* text_app, QWidget* parent)
  : ScintillaEdit(parent), m_fileWatcher(nullptr), m_currentUrlStart(-1),
  m_currentUrlEnd(-1), m_settingsDirty(true), m_textApp(text_app),
  m_filePath(QString()), m_encoding(DEFAULT_TEXT_FILE_ENCODING),
  m_lexer(text_app->settings()->syntaxHighlighting()->defaultLexer()) {

  connect(this, &TextEditor::marginClicked, this, &TextEditor::toggleFolding);
  connect(this, &TextEditor::modified, this, &TextEditor::onModified);
  connect(this, &TextEditor::notify, this, [this](SCNotification* pscn) {
    if (pscn->nmhdr.code == SCN_INDICATORCLICK && pscn->modifiers == SCMOD_CTRL) {
      qApp->web()->openUrlInExternalBrowser(textRange(m_currentUrlStart, m_currentUrlEnd));
    }
  });

  // TODO: idenntační linky
  //setIndentationGuides(SC_IV_REAL);

  // Set initial settings.
  setCodePage(SC_CP_UTF8);
  setMarginWidthN(MARGIN_SYMBOLS, 0);
  setWrapVisualFlags(SC_WRAPVISUALFLAG_MARGIN);
  setEndAtLastLine(false);
  setMultiPaste(SC_MULTIPASTE_EACH);
  setMultipleSelection(true);
  setEOLMode(m_textApp->settings()->eolMode());
}

void TextEditor::updateLineNumberMarginWidth(int zoom, QFont font, int line_count) {
  // Set point size and add some padding.
  font.setPointSize(font.pointSize() + zoom);

  QFontMetrics metr(font);
  int width = TextFactory::stringWidth(QString::number(line_count), metr) + MARGIN_PADDING_LINE_NUMBERS;

  setMarginWidthN(MARGIN_LINE_NUMBERS, width);
}

void TextEditor::loadFromFile(QFile& file, const QString& encoding, const Lexer& default_lexer, int initial_eol_mode) {
  m_filePath = QDir::toNativeSeparators(file.fileName());
  m_encoding = encoding.toLocal8Bit();
  m_lexer = default_lexer;

  setEOLMode(initial_eol_mode);

  QTextCodec* codec_for_encoding = QTextCodec::codecForName(m_encoding);

  if (codec_for_encoding == nullptr) {
    qCritical("We do not have codec for encoding '%s' when opening file, using defaults.", qPrintable(encoding));
    codec_for_encoding = QTextCodec::codecForName(QString(DEFAULT_TEXT_FILE_ENCODING).toLocal8Bit());
    m_encoding = codec_for_encoding->name();
  }

  QTextStream str(&file); str.setCodec(codec_for_encoding);

  blockSignals(true);
  setText(str.readAll().toUtf8().constData());
  emptyUndoBuffer();
  blockSignals(false);
  reattachWatcher(m_filePath);

  emit loadedFromFile(m_filePath);

  // We decide if this file is "log" file.
  setTargetRange(0, lineLength(0));
  setIsLog(searchInTarget(4, ".LOG") != -1);
}

void TextEditor::loadFromString(const QString& contents) {
  setText(contents.toUtf8().constData());
}

void TextEditor::onFileExternallyChanged(const QString& file_path) {
  Q_UNUSED(file_path)

  // File is externally changed.
  emit requestedVisibility();
  auto not_again = false;

  if (m_textApp->settings()->reloadModifiedDocumentsAutomatically() ||
      MessageBox::show(qApp->mainFormWidget(), QMessageBox::Icon::Question, tr("File externally modified"),
                       tr("File '%1' was modified outside of %2.").arg(QDir::toNativeSeparators(file_path),
                                                                       APP_NAME),
                       tr("Do you want to reload file now? This will discard all unsaved changes."),
                       QString(), QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No,
                       QMessageBox::StandardButton::Yes, &not_again, tr("Reload all files automatically (discard changes)")) ==
      QMessageBox::StandardButton::Yes) {

    reloadFromDisk();

    if (not_again) {
      m_textApp->settings()->setReloadModifiedDocumentsAutomatically(true);
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
    /*hoedown_renderer* renderer = hoedown_html_renderer_new(hoedown_html_flags::HOEDOWN_HTML_USE_XHTML, 0);
       hoedown_document* document = hoedown_document_new(renderer, hoedown_extensions::HOEDOWN_EXT_MATH, 16);
       hoedown_buffer* html = hoedown_buffer_new(this->length());
       uint8_t* data = new uint8_t[this->length()];

       for (int i = 0; i < this->length(); i++) {
       data[i] = charAt(i);
       }

       hoedown_document_render(document, html, data, this->length());

       QByteArray arr;

       for (int i = 0; i < html->size; i++) {
       arr.append(html->data[i]);
       }*/

    updateLineNumberMarginVisibility();
  }
}

void TextEditor::mouseMoveEvent(QMouseEvent* event) {
  Scintilla::Point mouse_pos = Scintilla::PointFromQPoint(event->pos());
  sptr_t text_pos = positionFromPointClose(mouse_pos.x, mouse_pos.y);

  if (text_pos == -1 || text_pos < m_currentUrlStart || text_pos > m_currentUrlEnd) {
    // We moved mouse pointer outside of existing URL indicator.
    // Thus, we remove current indicator
    if (m_currentUrlStart >= 0) {
      indicatorClearRange(m_currentUrlStart, m_currentUrlEnd);
      m_currentUrlStart = m_currentUrlEnd = -1;
    }

    if (text_pos >= 0) {

      // We find word separator on the left and on the right.
      sptr_t start = text_pos;
      sptr_t end = text_pos;
      int chr;

      while (true) {
        chr = charAt(start);

        if (!TextFactory::isCharUrlValid(chr)) {
          break;
        }
        else {
          start--;
        }
      }

      start++;

      while (true) {
        chr = charAt(end);

        if (!TextFactory::isCharUrlValid(chr)) {
          break;
        }
        else {
          end++;
        }
      }

      QByteArray ranged_text = textRange(start, end);
      QRegularExpressionMatch match = QRegularExpression(QSL("(https?:\\/\\/|ftp:\\/\\/|mailto:)[ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                                             "abcdefghijklmnopqrstuvwxyz0123456789\\-._~:\\/?#@!$&'*+,;=`.]+"))
                                      .match(ranged_text);

      if (match.hasMatch()) {
        // We found correct URL in hovered range.
        start = start + match.capturedStart();
        end = end - ranged_text.size() + match.capturedEnd();

        if (start != m_currentUrlStart || end != m_currentUrlEnd) {
          m_currentUrlStart = start;
          m_currentUrlEnd = end;

          setIndicatorCurrent(0);
          indicSetHoverStyle(0, INDIC_ROUNDBOX);
          indicatorFillRange(m_currentUrlStart, m_currentUrlEnd - m_currentUrlStart);
        }
      }
    }
  }

  ScintillaEdit::mouseMoveEvent(event);
}

void TextEditor::wheelEvent(QWheelEvent* event) {
  if (event->orientation() == Qt::Horizontal) {
    if (horizontalScrollBarPolicy() == Qt::ScrollBarAlwaysOff) {
      event->ignore();
    }
    else {
      QAbstractScrollArea::wheelEvent(event);
    }
  }
  else {
    if (QGuiApplication::keyboardModifiers() & Qt::ControlModifier) {
      if (event->delta() > 0) {
        m_textApp->settings()->increaseFontSize();
      }
      else {
        m_textApp->settings()->decreaseFontSize();
      }
    }
    else if (QGuiApplication::keyboardModifiers() & Qt::ShiftModifier) {
      if (event->delta() > 0) {
        m_textApp->settings()->increaseLineSpacing();
      }
      else {
        m_textApp->settings()->decreaseLineSpacing();
      }
    }
    else {
      if (verticalScrollBarPolicy() == Qt::ScrollBarAlwaysOff) {
        event->ignore();
      }
      else {
        QAbstractScrollArea::wheelEvent(event);
      }
    }
  }
}

void TextEditor::closeEvent(QCloseEvent* event) {
  bool ok = false;

  closeEditor(&ok);

  if (!ok) {
    event->ignore();
  }
  else {
    ScintillaEdit::closeEvent(event);
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

  if (styleFont(STYLE_DEFAULT) != new_font.family().toUtf8() ||
      styleSize(STYLE_DEFAULT) != new_font.pointSize() ||
      styleBold(STYLE_DEFAULT) != new_font.bold()) {
    styleSetFont(STYLE_DEFAULT, new_font.family().toUtf8().constData());
    styleSetSize(STYLE_DEFAULT, new_font.pointSize());
    styleSetBold(STYLE_DEFAULT, new_font.bold());
  }

  styleClearAll();

  // Now, we set some specific stuff.
  styleSetBold(STYLE_LINENUMBER, false);
  styleSetItalic(STYLE_LINENUMBER, false);
  styleSetWeight(STYLE_LINENUMBER, 1);

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

    m_settingsDirty = false;
  }
}

void TextEditor::reloadLexer(const Lexer& default_lexer) {
  m_lexer = default_lexer;
  setLexer(m_lexer.m_code);

  // Style with number 0 always black.
  styleSetFore(0, 0);

  // Gray whitespace characters.
  setWhitespaceFore(true, RGB_TO_SPRT(200, 200, 200));
  setWhitespaceSize(3);

  // Load more specific colors = keywords, operators etc.
  for (int i = 1; i <= STYLE_MAX; i++) {
    // We set colors for all non-predefined styles.
    if (m_lexer.m_code != SCLEX_NULL &&  (i < STYLE_DEFAULT || i > STYLE_LASTPREDEFINED)) {
      styleSetFore(i, RGB_TO_SPRT(rand() % 160, rand() % 160, rand() % 160));
    }
    else {
      styleSetFore(i, 0);
    }
  }

  // TODO: Setup folding, enable if some lexer is active, disable otherwise.

  /*if (m_lexer.m_code != SCLEX_NULL && m_lexer.m_code != SCLEX_CONTAINER) {
     // We activate folding.
     setProperty("fold", "1");
     setProperty("fold.compact", "1");
     setMarginWidthN(MARGIN_FOLDING, MARGIN_WIDTH_FOLDING);
     }
     else {
     setProperty("fold", "0");
     setProperty("fold.compact", "0");
     setMarginWidthN(MARGIN_FOLDING, 0);
     }

     setFoldFlags(SC_FOLDFLAG_LINEAFTER_CONTRACTED);
     setMarginSensitiveN(MARGIN_FOLDING, true);
     setMarginMaskN(MARGIN_FOLDING, SC_MASK_FOLDERS);
     markerDefine(SC_MARKNUM_FOLDER, SC_MARK_PLUS);
     markerDefine(SC_MARKNUM_FOLDEROPEN, SC_MARK_MINUS);
     markerDefine(SC_MARKNUM_FOLDEREND, SC_MARK_EMPTY);
     markerDefine(SC_MARKNUM_FOLDERMIDTAIL, SC_MARK_EMPTY);
     markerDefine(SC_MARKNUM_FOLDEROPENMID, SC_MARK_EMPTY);
     markerDefine(SC_MARKNUM_FOLDERSUB, SC_MARK_EMPTY);
     markerDefine(SC_MARKNUM_FOLDERTAIL, SC_MARK_EMPTY);
   */
  colourise(0, -1);
}

void TextEditor::saveToFile(const QString& file_path, bool* ok, const QString& encoding) {
  QFile file(file_path);

  reattachWatcher(QString());

  if (!file.open(QIODevice::Truncate | QIODevice::WriteOnly)) {
    *ok = false;
    reattachWatcher(m_filePath);
    return;
  }

  if (!encoding.isEmpty()) {
    m_encoding = encoding.toLocal8Bit();
  }

  QTextStream str(&file);

  str.setCodec(m_encoding.constData());
  str << getText(length() + 1);
  str.flush();
  file.close();

  m_filePath = QDir::toNativeSeparators(file_path);

  reattachWatcher(m_filePath);

  setSavePoint();
  emit savedToFile(m_filePath);

  *ok = true;
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

TextEditor* TextEditor::fromTextFile(TextApplication* app, const QString& file_path, const QString& explicit_encoding) {
  QFile file(file_path);

  if (!file.exists()) {
    qWarning("File '%s' does not exist and cannot be opened.", qPrintable(file_path));
    return nullptr;
  }

  if (file.size() >= MAX_TEXT_FILE_SIZE) {
    QMessageBox::critical(qApp->mainFormWidget(), tr("Cannot open file"),
                          tr("File '%1' too big. %2 can only open files smaller than %3 MB.").arg(QDir::toNativeSeparators(file_path),
                                                                                                  QSL(APP_NAME),
                                                                                                  QString::number(MAX_TEXT_FILE_SIZE /
                                                                                                                  1000000.0)));
    return nullptr;
  }

  if (!file.open(QIODevice::OpenModeFlag::ReadOnly)) {
    QMessageBox::critical(qApp->mainFormWidget(), tr("Cannot read file"),
                          tr("File '%1' cannot be opened for reading. Insufficient permissions.").arg(QDir::toNativeSeparators(file_path)));
    return nullptr;
  }

  QString encoding;
  Lexer default_lexer;
  int eol_mode = TextFactory::detectEol(file_path);

  if (eol_mode < 0) {
    qWarning("Auto-detection of EOL mode for file '%s' failed, using app default.", qPrintable(file_path));
    eol_mode = app->settings()->eolMode();
  }
  else {
    qDebug("Auto-detected EOL mode is '%d'.", eol_mode);
  }

  if (explicit_encoding.isEmpty()) {
    qDebug("No explicit encoding for file '%s'. Try to detect one.", qPrintable(file_path));

    if ((encoding = TextFactory::detectEncoding(file_path)).isEmpty()) {
      // No encoding auto-detected.
      encoding = DEFAULT_TEXT_FILE_ENCODING;
      qWarning("Auto-detection of encoding failed, using default encoding.");
    }
    else {
      qDebug("Auto-detected encoding is '%s'.", qPrintable(encoding));
    }
  }
  else {
    encoding = explicit_encoding;
  }

  if (file.size() > BIG_TEXT_FILE_SIZE) {
    if (encoding != DEFAULT_TEXT_FILE_ENCODING &&
        MessageBox::show(qApp->mainFormWidget(), QMessageBox::Question, tr("Opening big file"),
                         tr("You want to open big text file in encoding which is different from %1. This operation "
                            "might take quite some time.").arg(DEFAULT_TEXT_FILE_ENCODING),
                         tr("Do you really want to open the file?"),
                         QDir::toNativeSeparators(file_path), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::No) {
      return nullptr;
    }
    else {
      // File is quite big, we turn some features off to make sure it loads faster.
      QMessageBox::warning(qApp->mainFormWidget(), tr("Loading big file"),
                           tr("File '%1' is big. %2 will switch some features (for example 'Word wrap') off to "
                              "make sure that file loading is not horribly slow.").arg(QDir::toNativeSeparators(file_path),
                                                                                       QSL(APP_NAME)));

      app->settings()->setWordWrapEnabled(false);
    }
  }
  else {
    // We try to detect default lexer.
    default_lexer = app->settings()->syntaxHighlighting()->lexerForFile(file_path);
  }

  TextEditor* new_editor = new TextEditor(app, qApp->mainFormWidget());

  new_editor->loadFromFile(file, encoding, default_lexer, eol_mode);
  return new_editor;
}

void TextEditor::reloadFromDisk() {
  if (!filePath().isEmpty()) {
    if (modify()) {
      int answer = QMessageBox::question(qApp->mainFormWidget(), tr("Unsaved changes"),
                                         tr("File '%1' was externally changed, do you want to reload "
                                            "it from disk and discard any unsaved changes?"),
                                         QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No,
                                         QMessageBox::StandardButton::Yes);

      if (answer != QMessageBox::StandardButton::Yes) {
        return;
      }
    }

    // We reload now.
    // When we reload, we automatically detect EOL and encoding
    // and show no warnings, makes no sense in this use-case.
    QFile file(filePath());

    if (!file.exists()) {
      qWarning("File '%s' does not exist and cannot be opened.", qPrintable(filePath()));
      return;
    }

    if (file.size() >= MAX_TEXT_FILE_SIZE) {
      QMessageBox::critical(qApp->mainFormWidget(), tr("Cannot open file"),
                            tr("File '%1' too big. %2 can only open files smaller than %3 MB.").arg(QDir::toNativeSeparators(filePath()),
                                                                                                    QSL(APP_NAME),
                                                                                                    QString::number(MAX_TEXT_FILE_SIZE /
                                                                                                                    1000000.0)));
      return;
    }

    if (!file.open(QIODevice::OpenModeFlag::ReadOnly)) {
      QMessageBox::critical(qApp->mainFormWidget(),
                            tr("Cannot read file"),
                            tr("File '%1' cannot be opened for reading. Insufficient permissions.").arg(QDir::toNativeSeparators(
                                                                                                          filePath())));
      return;
    }

    QString encoding;
    Lexer default_lexer;
    int eol_mode = TextFactory::detectEol(filePath());

    if (eol_mode < 0) {
      qWarning("Auto-detection of EOL mode for file '%s' failed, using app default.", qPrintable(filePath()));
      eol_mode = m_textApp->settings()->eolMode();
    }
    else {
      qDebug("Auto-detected EOL mode is '%d'.", eol_mode);
    }

    qDebug("No explicit encoding for file '%s'. Try to detect one.", qPrintable(filePath()));

    if ((encoding = TextFactory::detectEncoding(filePath())).isEmpty()) {
      // No encoding auto-detected.
      encoding = DEFAULT_TEXT_FILE_ENCODING;
      qWarning("Auto-detection of encoding failed, using default encoding.");
    }
    else {
      qDebug("Auto-detected encoding is '%s'.", qPrintable(encoding));
    }

    if (file.size() > BIG_TEXT_FILE_SIZE) {
      if (encoding != DEFAULT_TEXT_FILE_ENCODING &&
          MessageBox::show(qApp->mainFormWidget(), QMessageBox::Question, tr("Opening big file"),
                           tr("You want to open big text file in encoding which is different from %1. This operation "
                              "might take quite some time.").arg(DEFAULT_TEXT_FILE_ENCODING),
                           tr("Do you really want to open the file?"),
                           QDir::toNativeSeparators(filePath()), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::No) {
        return;
      }
      else {
        // File is quite big, we turn some features off to make sure it loads faster.
        QMessageBox::warning(qApp->mainFormWidget(), tr("Loading big file"),
                             tr("File '%1' is big. %2 will switch some features (for example 'Word wrap') off to "
                                "make sure that file loading is not horribly slow.").arg(QDir::toNativeSeparators(filePath()),
                                                                                         QSL(APP_NAME)));

        m_textApp->settings()->setWordWrapEnabled(false);
      }
    }
    else {
      // We try to detect default lexer.
      default_lexer = m_textApp->settings()->syntaxHighlighting()->lexerForFile(filePath());
    }

    loadFromFile(file, encoding, default_lexer, eol_mode);
    emit editorReloaded();
  }
}

void TextEditor::setEncoding(const QByteArray& encoding) {
  m_encoding = encoding;
}

void TextEditor::updateLineNumberMarginVisibility() {
  const int current_width = marginWidthN(MARGIN_LINE_NUMBERS);
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
  }
}

void TextEditor::toggleFolding(int position, int modifiers, int margin) {
  Q_UNUSED(modifiers)

  const int line_number = lineFromPosition(position);

  switch (margin) {
    case MARGIN_FOLDING:
      toggleFold(line_number);
      break;

    default:
      break;
  }
}

void TextEditor::printPreview() {
  TextEditorPrinter printer;

  printer.setZoom(-2);

  QPrintPreviewDialog dialog(&printer, qApp->mainFormWidget());

  connect(&dialog, &QPrintPreviewDialog::paintRequested, this, [this](QPrinter* prntr) {
    TextEditorPrinter* sndr = static_cast<TextEditorPrinter*>(prntr);
    sndr->printRange(this);
  });

  dialog.exec();
}

void TextEditor::print() {
  TextEditorPrinter printer;

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

void TextEditor::save(bool* ok) {
  if (m_filePath.isEmpty()) {
    // Newly created document, save as.
    saveAs(ok);
  }
  else {
    // We just save this modified document to same file.
    saveToFile(m_filePath, ok);
  }
}

void TextEditor::saveAs(bool* ok, const QString& encoding) {
  // We save this documents as new file.
  QString file_path = MessageBox::getSaveFileName(qApp->mainFormWidget(),
                                                  tr("Save file as"),
                                                  m_filePath.isEmpty() ?
                                                  m_textApp->settings()->loadSaveDefaultDirectory() :
                                                  QFileInfo(m_filePath).absolutePath(),
                                                  QFileInfo(m_filePath).fileName(),
                                                  m_textApp->settings()->syntaxHighlighting()->fileFilters(),
                                                  nullptr);

  if (!file_path.isEmpty()) {
    m_textApp->settings()->setLoadSaveDefaultDirectory(file_path);

    if (encoding.isEmpty()) {
      saveToFile(file_path, ok);
    }
    else {
      saveToFile(file_path, ok, encoding);
    }
  }
  else {
    *ok = false;
  }
}

void TextEditor::closeEditor(bool* ok) {
  if (modify()) {
    emit requestedVisibility();

    // We need to save.
    QMessageBox::StandardButton response = QMessageBox::question(qApp->mainFormWidget(),
                                                                 tr("Unsaved changes"),
                                                                 tr("This document has unsaved changes, do you want to save them?"),
                                                                 QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
                                                                 QMessageBox::Save);

    switch (response) {
      case QMessageBox::StandardButton::Save: {
        bool ok_save = false;

        save(&ok_save);
        *ok = ok_save;
        break;
      }

      case QMessageBox::StandardButton::Discard:
        *ok = true;
        break;

      case QMessageBox::StandardButton::Cancel:
        *ok = false;
        break;

      default:
        *ok = false;
        break;
    }
  }
  else {
    *ok = true;
  }
}

QString TextEditor::filePath() const {
  return m_filePath;
}
