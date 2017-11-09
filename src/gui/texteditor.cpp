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

#include "scintilla/include/SciLexer.h"

#include <QDir>
#include <QFileDialog>
#include <QFontDatabase>
#include <QTextCodec>
#include <QTextStream>

TextEditor::TextEditor(TextApplication* text_app, QWidget* parent) : ScintillaEdit(parent), m_textApp(text_app),
  m_filePath(QString()), m_encoding(DEFAULT_TEXT_FILE_ENCODING),
  m_lexer(text_app->settings()->syntaxHighlighting()->defaultLexer()) {
  setCodePage(SC_CP_UTF8);
}

void TextEditor::loadFromFile(QFile& file, const QString& encoding, const Lexer& default_lexer) {
  m_filePath = file.fileName();
  m_encoding = encoding.toLocal8Bit();
  m_lexer = default_lexer;

  //reloadLexer(default_lexer);

  Application::setOverrideCursor(Qt::CursorShape::WaitCursor);

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

  Application::restoreOverrideCursor();

  emit loadedFromFile(m_filePath);
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

const char htmlKeyWords[] =
  "a abbr acronym address applet area b base basefont "
  "bdo big blockquote body br button caption center "
  "cite code col colgroup dd del dfn dir div dl dt em "
  "fieldset font form frame frameset h1 h2 h3 h4 h5 h6 "
  "head hr html i iframe img input ins isindex kbd label "
  "legend li link map menu meta noframes noscript "
  "object ol optgroup option p param pre q s samp "
  "script select small span strike strong style sub sup "
  "table tbody td textarea tfoot th thead title tr tt u ul "
  "var xmlns "
  "abbr accept-charset accept accesskey action align alink "
  "alt archive axis background bgcolor border "
  "cellpadding cellspacing char charoff charset checked cite "
  "class classid clear codebase codetype color cols colspan "
  "compact content coords "
  "data datafld dataformatas datapagesize datasrc datetime "
  "declare defer dir disabled enctype "
  "face for frame frameborder "
  "headers height href hreflang hspace http-equiv "
  "id ismap label lang language link longdesc "
  "marginwidth marginheight maxlength media method multiple "
  "name nohref noresize noshade nowrap "
  "object onblur onchange onclick ondblclick onfocus "
  "onkeydown onkeypress onkeyup onload onmousedown "
  "onmousemove onmouseover onmouseout onmouseup "
  "onreset onselect onsubmit onunload "
  "profile prompt readonly rel rev rows rowspan rules "
  "scheme scope shape size span src standby start style "
  "summary tabindex target text title type usemap "
  "valign value valuetype version vlink vspace width "
  "text password checkbox radio submit reset "
  "file hidden image "
  "public !doctype xml";
void TextEditor::reloadLexer(const Lexer& default_lexer) {
  m_lexer = default_lexer;

  setLexerLanguage("html");

  setProperty("fold", "1");
  setProperty("fold.html", "1");

  setKeyWords(0, htmlKeyWords);
  styleClearAll();
  styleSetFore(SCE_H_ATTRIBUTE, 255);

  colourise(0, -1);

/*
   clearDocumentStyle();
   setLexer(SCLEX_CPP);
   setProperty("fold", "1");
   setProperty("fold.html", "1");

   colourise(0, -1);
 */

  // TODO: dodělat korektně

  /*
     QsciLexer* old_lexer = QsciScintilla::lexer();
     QsciLexer* new_lexer = m_lexer.m_lexerGenerator();

     if (new_lexer != old_lexer) {
     setLexer(new_lexer);
     qDebug("Changing lexers from '%s' to '%s'.",
           old_lexer == nullptr ? qPrintable(QSL("nothing")) : qPrintable(old_lexer->language()),
           new_lexer == nullptr ? qPrintable(QSL("nothing")) : qPrintable(new_lexer->language()));

     if (old_lexer != nullptr) {
      old_lexer->deleteLater();
     }
     }*/

  reloadFont();
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

  str << getText(length());
  str.flush();
  file.close();

  setSavePoint();
  emit savedToFile((m_filePath = file_path));

  *ok = true;
}

Lexer TextEditor::lexer() const {
  return m_lexer;
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
  else if (modify()) {
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

void TextEditor::reloadFont() {
  QFont new_font = textApplication()->settings()->mainFont();

  if (styleFont(STYLE_DEFAULT) != new_font.family().toUtf8() || styleSize(STYLE_DEFAULT) != new_font.pointSize()) {
    styleSetFont(STYLE_DEFAULT, new_font.family().toUtf8().constData());
    styleSetSize(STYLE_DEFAULT, new_font.pointSize());
    styleClearAll();
  }
}

void TextEditor::closeEditor(bool* ok) {
  if (modify()) {
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
  setEOLMode(m_textApp->settings()->eolMode());
  setWrapVisualFlags(SC_WRAPVISUALFLAG_MARGIN);
  setWrapMode(m_textApp->settings()->wordWrapEnabled() ? SC_WRAP_WORD : SC_WRAP_NONE);

  setMarginWidthN(MARGIN_LINE_NUMBERS, MARGIN_WIDTH_NUMBERS);

  //setMarginWidthN(MARGIN_FOLDING, MARGIN_WIDTH_FOLDING);
  setViewEOL(m_textApp->settings()->viewEols());
  setViewWS(m_textApp->settings()->viewWhitespaces() ? SCWS_VISIBLEALWAYS : SCWS_INVISIBLE);

  reloadLexer(m_lexer);

  setCaretLineBack(255);
  setCaretLineVisible(true);
  setCaretWidth(2);
}

QString TextEditor::filePath() const {
  return m_filePath;
}
