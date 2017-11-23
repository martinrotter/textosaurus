// For license of this file, see <project-root-folder>/LICENSE.md.

#include "gui/texteditor.h"

#include "definitions/definitions.h"
#include "exceptions/ioexception.h"
#include "gui/messagebox.h"
#include "miscellaneous/application.h"
#include "miscellaneous/iofactory.h"
#include "miscellaneous/syntaxhighlighting.h"
#include "miscellaneous/textapplication.h"
#include "miscellaneous/textapplicationsettings.h"

#include "scintilla/include/ILoader.h"
#include "scintilla/include/SciLexer.h"

#include <QDir>
#include <QFileDialog>
#include <QFontDatabase>
#include <QTextCodec>
#include <QTextStream>

TextEditor::TextEditor(TextApplication* text_app, QWidget* parent) : ScintillaEdit(parent), m_textApp(text_app),
  m_filePath(QString()), m_encoding(DEFAULT_TEXT_FILE_ENCODING),
  m_lexer(text_app->settings()->syntaxHighlighting()->defaultLexer()) {

  connect(this, &TextEditor::marginClicked, this, [this](int position, int modifiers, int margin) {
    Q_UNUSED(modifiers)

    const int line_number = lineFromPosition(position);

    switch (margin) {
      case MARGIN_FOLDING:
        toggleFold(line_number);
        break;

      default:
        break;
    }
  });

  // Set initial settings.
  setEOLMode(m_textApp->settings()->eolMode());
  setCodePage(SC_CP_UTF8);
  setWrapVisualFlags(SC_WRAPVISUALFLAG_MARGIN);
  setMarginWidthN(MARGIN_LINE_NUMBERS, MARGIN_WIDTH_NUMBERS);
  setEndAtLastLine(false);
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

  emit loadedFromFile(m_filePath);
}

void TextEditor::loadFromString(const QString& contents) {
  setText(contents.toUtf8().constData());
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
  styleSetWeight(STYLE_LINENUMBER, 100);
}

void TextEditor::reloadSettings() {
  setWrapMode(m_textApp->settings()->wordWrapEnabled() ? SC_WRAP_WORD : SC_WRAP_NONE);
  setViewEOL(m_textApp->settings()->viewEols());
  setViewWS(m_textApp->settings()->viewWhitespaces() ? SCWS_VISIBLEALWAYS : SCWS_INVISIBLE);

  reloadLexer(m_lexer);
}

void TextEditor::reloadLexer(const Lexer& default_lexer) {
  m_lexer = default_lexer;

  reloadFont();
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
  if (m_lexer.m_code != SCLEX_NULL && m_lexer.m_code != SCLEX_CONTAINER) {
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

  colourise(0, -1);
}

void TextEditor::saveToFile(const QString& file_path, bool* ok, const QString& encoding) {
  if (!encoding.isEmpty()) {
    m_encoding = encoding.toLocal8Bit();
  }

  QFile file(file_path);

  if (!file.open(QIODevice::Truncate | QIODevice::WriteOnly)) {
    *ok = false;
    return;
  }

  QTextStream str(&file); str.setCodec(m_encoding.constData());

  str << getText(length() + 1);
  str.flush();
  file.close();

  m_filePath = QDir::toNativeSeparators(file_path);

  setSavePoint();
  emit savedToFile(m_filePath);

  *ok = true;
}

void TextEditor::setEncoding(const QByteArray& encoding) {
  m_encoding = encoding;
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
