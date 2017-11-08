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

#include <QDir>
#include <QFileDialog>
#include <QFontDatabase>
#include <QTextCodec>
#include <QTextStream>

#include <Qsci/qscilexercpp.h>

TextEditor::TextEditor(TextApplication* text_app, QWidget* parent) : QsciScintilla(parent), m_textApp(text_app),
  m_filePath(QString()), m_encoding(DEFAULT_TEXT_FILE_ENCODING) {
  setUtf8(true);
}

void TextEditor::loadFromFile(QFile& file, const QString& encoding, QsciLexer* default_lexer) {
  m_filePath = file.fileName();
  m_encoding = encoding.toLocal8Bit();

  if (lexer() != nullptr) {
    lexer()->deleteLater();
  }

  if (default_lexer != nullptr) {
    setLexer(default_lexer);
  }

  Application::setOverrideCursor(Qt::CursorShape::WaitCursor);

  QTextCodec* codec_for_encoding = QTextCodec::codecForName(m_encoding);

  if (codec_for_encoding == nullptr) {
    qCritical("We do not have codec for encoding '%s' when opening file, using defaults.", qPrintable(encoding));
    codec_for_encoding = QTextCodec::codecForName(QString(DEFAULT_TEXT_FILE_ENCODING).toLocal8Bit());
    m_encoding = codec_for_encoding->name();
  }

  QTextStream str(&file); str.setCodec(codec_for_encoding);
  QString next_line;

  blockSignals(true);

  while (!(next_line = str.read(50000000)).isEmpty()) {
    append(next_line);
  }

  blockSignals(false);
  Application::restoreOverrideCursor();
  emit loadedFromFile(m_filePath);
}

void TextEditor::contextMenuEvent(QContextMenuEvent* event) {
  QsciScintilla::contextMenuEvent(event);
}

void TextEditor::closeEvent(QCloseEvent* event) {
  bool ok = false;

  closeEditor(&ok);

  if (!ok) {
    event->ignore();
  }
  else {
    QsciScintilla::closeEvent(event);
  }
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

  str << text();

  emit savedToFile(m_filePath);

  setModified(false);
  *ok = true;
}

TextApplication* TextEditor::textApplication() const {
  return m_textApp;
}

QByteArray TextEditor::encoding() const {
  return m_encoding;
}

void TextEditor::save(bool* ok) {
  if (m_filePath.isEmpty()) {
    // Newly created document, save as.
    saveAs(ok);
  }
  else if (isModified()) {
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
                                                  textApplication()->settings()->syntaxHighlighting()->fileFilters(),
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
  if (isModified()) {
    emit requestVisibility();

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

void TextEditor::reloadSettings() {
  setEolMode(m_textApp->settings()->eolMode());
  setWrapVisualFlags(QsciScintilla::WrapVisualFlag::WrapFlagNone, QsciScintilla::WrapVisualFlag::WrapFlagInMargin);
  setWrapMode(m_textApp->settings()->wordWrapEnabled() ? QsciScintilla::WrapMode::WrapWord : QsciScintilla::WrapMode::WrapNone);

  setMarginWidth(MARGIN_LINE_NUMBERS, MARGIN_WIDTH_NUMBERS);
  setMarginWidth(MARGIN_FOLDING, MARGIN_WIDTH_FOLDING);

  setEolVisibility(m_textApp->settings()->viewEols());
  setWhitespaceVisibility(m_textApp->settings()->viewWhitespaces() ?
                          QsciScintilla::WhitespaceVisibility::WsVisible :
                          QsciScintilla::WhitespaceVisibility::WsInvisible);
  setAutoIndent(true);

  setCaretLineBackgroundColor(QColor(230, 230, 230));
  setCaretLineVisible(true);
  setCaretWidth(2);

  //setLexer(new QsciLexerCPP(this));
  setFolding(QsciScintilla::FoldStyle::PlainFoldStyle);

  lexer()->setFont(QFontDatabase::systemFont(QFontDatabase::SystemFont::FixedFont));

  setAutoCompletionCaseSensitivity(false);
  setAutoCompletionThreshold(0);
  setAutoCompletionFillupsEnabled(true);
  setAutoCompletionSource(QsciScintilla::AcsAll);
}

QString TextEditor::filePath() const {
  return m_filePath;
}
