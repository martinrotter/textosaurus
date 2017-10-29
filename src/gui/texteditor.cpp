// For license of this file, see <project-root-folder>/LICENSE.md.

#include "gui/texteditor.h"

#include "definitions/definitions.h"
#include "exceptions/ioexception.h"
#include "miscellaneous/application.h"
#include "miscellaneous/iofactory.h"

#include <QDir>
#include <QFileDialog>
#include <QFontDatabase>
#include <QtConcurrent/QtConcurrentRun>
#include <QTextCodec>
#include <QTextStream>

TextEditor::TextEditor(QWidget* parent) : QsciScintilla(parent), m_filePath(QString()), m_encoding(QByteArray()) {
  reloadSettings();
}

void TextEditor::loadFromFile(QFile& file, const QString& encoding) {
  m_filePath = file.fileName();

  Application::setOverrideCursor(Qt::CursorShape::WaitCursor);

  QTextStream str(&file); str.setCodec(m_encoding = encoding.toLatin1().constData());
  QString next_line;

  blockSignals(true);

  while (!(next_line = str.read(50000000)).isEmpty()) {
    append(next_line);
  }

  blockSignals(false);

  Application::restoreOverrideCursor();

  emit loadedFromFile(m_filePath);
}

void TextEditor::closeEvent(QCloseEvent* event) {
  bool ok;

  closeEditor(ok);

  if (!ok) {
    event->ignore();
  }
  else {
    QsciScintilla::closeEvent(event);
  }
}

void TextEditor::saveToFile(const QString& file_path) {
  IOFactory::writeFile(file_path, text().toUtf8());

  if (m_filePath != file_path) {
    m_filePath = file_path;
    emit savedToFile(m_filePath);
  }

  setModified(false);
}

QByteArray TextEditor::encoding() const {
  return m_encoding;
}

void TextEditor::save(bool& ok) {
  if (m_filePath.isEmpty()) {
    // Newly created document, save as.
    saveAs(ok);
  }
  else if (isModified()) {
    // We just save this modified document to same file.
    saveToFile(m_filePath);
  }
}

void TextEditor::saveAs(bool& ok) {
  // We save this documents as new file.
  QString file_path = QFileDialog::getSaveFileName(qApp->mainFormWidget(), tr("Save file as"),
                                                   qApp->documentsFolder(), QSL("Text files (*.txt);;All files (*)"));

  if (!file_path.isEmpty()) {
    saveToFile(file_path);
    ok = true;
  }
  else {
    ok = false;
  }
}

void TextEditor::closeEditor(bool& ok) {
  if (isModified()) {
    emit requestVisibility();

    // We need to save.
    QMessageBox::StandardButton response = QMessageBox::question(qApp->mainFormWidget(),
                                                                 tr("Unsaved changed"),
                                                                 tr("This document has unsaved changed, do you want to save them?"),
                                                                 QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
                                                                 QMessageBox::Save);

    switch (response) {
      case QMessageBox::StandardButton::Save:
        bool ok_save;

        save(ok_save);
        ok = ok_save;
        break;

      case QMessageBox::StandardButton::Discard:
        ok = true;
        break;

      case QMessageBox::StandardButton::Cancel:
        ok = false;
        break;

      default:
        ok = false;
        break;
    }
  }
  else {
    ok = true;
  }
}

void TextEditor::reloadSettings() {
  // TODO: nacist font atd.
  setUtf8(true);
  setFont(QFontDatabase::systemFont(QFontDatabase::SystemFont::FixedFont));
}

QString TextEditor::filePath() const {
  return m_filePath;
}
