// For license of this file, see <project-root-folder>/LICENSE.md.

#include "gui/texteditorprinter.h"

#include "gui/texteditor.h"
#include "scintilla/include/Scintilla.h"

#include <QPainter>
#include <QStack>

TextEditorPrinter::TextEditorPrinter() : QPrinter(), m_zoom(0), m_wrapMode(SC_WRAP_WORD) {}

TextEditorPrinter::~TextEditorPrinter() {}

int TextEditorPrinter::printRange(TextEditor* qsb, int from, int to) {
  // Sanity check.
  if (qsb == nullptr) {
    return false;
  }

  // Setup the printing area.
  QRect def_area;

  def_area.setX(0);
  def_area.setY(0);
  def_area.setWidth(width());
  def_area.setHeight(height());

  // Get the page range.
  int pgFrom, pgTo;

  pgFrom = fromPage();
  pgTo = toPage();

  // Find the position range.
  long startPos, endPos;

  endPos = qsb->length();
  startPos = from > 0 ? qsb->positionFromLine(from) : 0;

  if (to >= 0) {
    long toPos = qsb->positionFromLine(to + 1);

    if (endPos > toPos) {
      endPos = toPos;
    }
  }

  if (startPos >= endPos) {
    return false;
  }

  QPainter painter(this);
  bool reverse = pageOrder() == PageOrder::LastPageFirst;
  bool needNewPage = false;

  qsb->setPrintMagnification(m_zoom);
  qsb->setPrintWrapMode(m_wrapMode);

  for (int i = 1; i <= numCopies(); ++i) {
    // If we are printing in reverse page order then remember the start
    // position of each page.
    QStack<long> pageStarts;

    int currPage = 1;
    long pos = startPos;

    while (pos < endPos) {
      // See if we have finished the requested page range.
      if (pgTo > 0 && pgTo < currPage) {
        break;
      }

      // See if we are going to render this page, or just see how much
      // would fit onto it.
      bool render = false;

      if (pgFrom == 0 || pgFrom <= currPage) {
        if (reverse) {
          pageStarts.push(pos);
        }
        else {
          render = true;

          if (needNewPage) {
            if (!newPage()) {
              return false;
            }
          }
          else {
            needNewPage = true;
          }
        }
      }

      QRect area = def_area;

      pos = qsb->formatRange(render, this, this, area, paperRect(), pos, endPos);

      ++currPage;
    }

    // All done if we are printing in normal page order.
    if (!reverse) {
      continue;
    }

    // Now go through each page on the stack and really print it.
    while (!pageStarts.isEmpty()) {
      --currPage;

      long ePos = pos;

      pos = pageStarts.pop();

      if (needNewPage) {
        if (!newPage()) {
          return false;
        }
      }
      else {
        needNewPage = true;
      }

      QRect area = def_area;

      qsb->formatRange(true, this, this, area, paperRect(), pos, ePos);

      //qsb->SendScintilla(QsciScintillaBase::SCI_FORMATRANGE, true, &painter, area, pos, ePos);
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
