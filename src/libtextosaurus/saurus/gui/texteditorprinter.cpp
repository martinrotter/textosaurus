// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "saurus/gui/texteditorprinter.h"

#include "definitions/definitions.h"

// *INDENT-OFF*
#include PATH(SCINTILLA_DIR,qt/ScintillaEdit/ScintillaEdit.h)

// *INDENT-ON*

#include "saurus/gui/texteditor.h"

#include <QPainter>
#include <QStack>

TextEditorPrinter::TextEditorPrinter() : m_zoom(0), m_wrapMode(SC_WRAP_WORD) {}

bool TextEditorPrinter::printRange(TextEditor* editor, int from, int to) {
  // Sanity check.
  if (editor == nullptr) {
    return false;
  }

  // Setup the printing area.
  QRect def_area;

  def_area.setX(0);
  def_area.setY(0);
  def_area.setWidth(width());
  def_area.setHeight(height());

  // Get the page range.
  int pg_from, pg_to;

  pg_from = fromPage();
  pg_to = toPage();

  // Find the position range.
  long start_pos, end_pos;

  end_pos = long(editor->length());
  start_pos = from > 0 ? long(editor->positionFromLine(from)) : 0;

  if (to >= 0) {
    long to_os = long(editor->positionFromLine(to + 1));

    if (end_pos > to_os) {
      end_pos = to_os;
    }
  }

  if (start_pos >= end_pos) {
    return false;
  }

  QPainter painter(this);
  bool reverse = pageOrder() == PageOrder::LastPageFirst;
  bool need_new_page = false;

  editor->setPrintMagnification(m_zoom);
  editor->setPrintWrapMode(m_wrapMode);

  for (int i = 1; i <= numCopies(); ++i) {
    // If we are printing in reverse page order then remember the start
    // position of each page.
    QStack<long> page_starts;

    int curr_page = 1;
    long pos = start_pos;

    while (pos < end_pos) {
      // See if we have finished the requested page range.
      if (pg_to > 0 && pg_to < curr_page) {
        break;
      }

      // See if we are going to render this page, or just see how much
      // would fit onto it.
      bool render = false;

      if (pg_from == 0 || pg_from <= curr_page) {
        if (reverse) {
          page_starts.push(pos);
        }
        else {
          render = true;

          if (need_new_page) {
            if (!newPage()) {
              return false;
            }
          }
          else {
            need_new_page = true;
          }
        }
      }

      QRect area = def_area;

      pos = editor->formatRange(render, this, this, area, paperRect(), pos, end_pos);

      ++curr_page;
    }

    // All done if we are printing in normal page order.
    if (!reverse) {
      continue;
    }

    // Now go through each page on the stack and really print it.
    while (!page_starts.isEmpty()) {
      --curr_page;

      long e_pos = pos;

      pos = page_starts.pop();

      if (need_new_page) {
        if (!newPage()) {
          return false;
        }
      }
      else {
        need_new_page = true;
      }

      QRect area = def_area;

      editor->formatRange(true, this, this, area, paperRect(), pos, e_pos);
    }
  }

  return true;
}

int TextEditorPrinter::zoom() const {
  return m_zoom;
}

void TextEditorPrinter::setZoom(int zoom) {
  m_zoom = zoom;
}

int TextEditorPrinter::wrapMode() const {
  return m_wrapMode;
}

void TextEditorPrinter::setWrapMode(int wrap_mode) {
  m_wrapMode = wrap_mode;
}
