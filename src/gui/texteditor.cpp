// For license of this file, see <project-root-folder>/LICENSE.md.

#include "gui/texteditor.h"

#include "definitions/definitions.h"
#include "exceptions/ioexception.h"
#include "miscellaneous/application.h"
#include "miscellaneous/iofactory.h"

#include <QDir>
#include <QtConcurrent/QtConcurrentRun>
#include <QTextCodec>
#include <QTextStream>

TextEditor::TextEditor(QWidget* parent) : QsciScintilla(parent), m_filePath(QString()), m_encoding(QByteArray()) {
  setUtf8(true);
  setFont(QFont("Dejavu Sans Mono"));
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

QByteArray TextEditor::encoding() const
{
  return m_encoding;
}

QString TextEditor::filePath() const {
  return m_filePath;
}
