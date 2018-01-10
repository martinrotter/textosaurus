// For license of this file, see <project-root-folder>/LICENSE.md.

#include "gui/sidebars/markdownsidebar.h"

MarkdownSidebar::MarkdownSidebar(TextApplication* app, QWidget* parent) : DockWidget(parent), m_textApp(app) {
  setWindowTitle(tr("Markdown"));
}

Qt::DockWidgetArea MarkdownSidebar::initialArea() const {
  return Qt::DockWidgetArea::RightDockWidgetArea;
}

bool MarkdownSidebar::initiallyVisible() const {
  return false;
}

/*    hoedown_renderer* renderer = hoedown_html_renderer_new(hoedown_html_flags::HOEDOWN_HTML_USE_XHTML, 0);
    hoedown_document* document = hoedown_document_new(renderer, hoedown_extensions::HOEDOWN_EXT_MATH, 16);
    hoedown_buffer* html = hoedown_buffer_new(this->length());
    uint8_t* direct_editor_data = (uint8_t*)characterPointer();
    size_t delka = strlen((char*)direct_editor_data);

    hoedown_document_render(document, html, direct_editor_data, delka);

    QString arr = QString::fromUtf8((char*)html->data, html->size);

    QMessageBox::warning(nullptr, arr, arr);*/

int MarkdownSidebar::initialWidth() const {
  return 150;
}

void MarkdownSidebar::load() {}
