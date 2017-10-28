// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H

#include <Qsci/qsciscintilla.h>

class TextEditor : public QsciScintilla {
  Q_OBJECT

  public:
    explicit TextEditor();
};

#endif // TEXTEDITOR_H
