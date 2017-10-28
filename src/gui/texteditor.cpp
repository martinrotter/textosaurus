// For license of this file, see <project-root-folder>/LICENSE.md.

#include "gui/texteditor.h"

#include "definitions/definitions.h"
#include "exceptions/ioexception.h"
#include "miscellaneous/iofactory.h"

#include <QDir>

TextEditor::TextEditor(QWidget* parent) : QsciScintilla(parent), m_filePath(QString()) {}

void TextEditor::loadFromFile(QFile& file) {
  m_filePath = file.fileName();

  setText(IOFactory::readFile(file.fileName()));
}

QString TextEditor::filePath() const {
  return m_filePath;
}

void TextEditor::setFilePath(const QString& file_path) {
  m_filePath = file_path;
}
