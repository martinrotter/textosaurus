// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef TEXTEDITORPRINTER_H
#define TEXTEDITORPRINTER_H

#include <QPrinter>

class TextEditor;

class TextEditorPrinter : public QPrinter {
  public:
    explicit TextEditorPrinter();
    virtual ~TextEditorPrinter();

    virtual int printRange(TextEditor* editor, int from = -1, int to = -1);

    int zoom() const;
    void setZoom(int zoom);

    int wrapMode() const;
    void setWrapMode(int wrap_mode);

  private:
    int m_zoom;
    int m_wrapMode;
};

#endif // TEXTEDITORPRINTER_H
