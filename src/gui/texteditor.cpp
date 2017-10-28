// For license of this file, see <project-root-folder>/LICENSE.md.

#include "gui/texteditor.h"

#include "definitions/definitions.h"
#include "exceptions/ioexception.h"
#include "miscellaneous/application.h"
#include "miscellaneous/iofactory.h"

#include <QDir>
#include <QTextCodec>
#include <QTextStream>

TextEditor::TextEditor(QWidget* parent) : QsciScintilla(parent), m_filePath(QString()) {
  setUtf8(true);
  setFont(QFont("Dejavu Sans Mono"));
}

void TextEditor::loadFromFile(QFile& file) {
  m_filePath = file.fileName();

  file.open(QIODevice::OpenModeFlag::ReadOnly);
  QTextStream str(&file);

  str.setCodec("utf-8");

  Application::setOverrideCursor(Qt::CursorShape::WaitCursor);
  QString stra = str.readAll();

  Application::restoreOverrideCursor();

  setText(stra);
}

QString TextEditor::filePath() const {
  return m_filePath;
}
