// For license of this file, see <project-root-folder>/LICENSE.md.

#include "plugin-system/markdown/markdownsidebar.h"

#include "gui/texteditor.h"

#include "miscellaneous/application.h"
#include "miscellaneous/iconfactory.h"
#include "miscellaneous/textapplication.h"

#include "3rd-party/hoedown/hdocument.h"
#include "3rd-party/hoedown/html.h"

#include <QTextBrowser>
#include <QToolBar>
#include <QVBoxLayout>

MarkdownSidebar::MarkdownSidebar(TextApplication* text_app, QWidget* parent)
  : BaseSidebar(text_app, parent), m_txtPreview(nullptr) {
  setWindowTitle(tr("Markdown Preview"));
  setObjectName(QSL("m_sidebarMarkdown"));

  m_actionRefreshPreview = new QAction(qApp->icons()->fromTheme(QSL("view-refresh")),
                                       tr("Refresh Markdown Preview"),
                                       this);
  m_actionRefreshPreview->setObjectName(QSL("m_actionRefreshMarkdownPreview"));
  connect(m_actionRefreshPreview, &QAction::triggered, this, &MarkdownSidebar::refreshPreview);
}

Qt::DockWidgetArea MarkdownSidebar::initialArea() const {
  return Qt::DockWidgetArea::RightDockWidgetArea;
}

bool MarkdownSidebar::initiallyVisible() const {
  return false;
}

int MarkdownSidebar::initialWidth() const {
  return 150;
}

void MarkdownSidebar::refreshPreview() {
  show();
  raise();

  TextEditor* editor = m_textApp->currentEditor();

  if (editor != nullptr) {
    m_txtPreview->setText(convertMarkdownToHtml((const uint8_t*)editor->characterPointer()));
  }
  else {
    m_txtPreview->clear();
  }
}

void MarkdownSidebar::load() {
  if (m_txtPreview == nullptr) {
    m_txtPreview = new QTextBrowser(this);
    m_txtPreview->setPlaceholderText(tr("This sidebar displays simple HTML preview of your Markdown source..."));
    m_txtPreview->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    m_txtPreview->setWordWrapMode(QTextOption::WrapMode::WrapAnywhere);
    m_txtPreview->setReadOnly(true);
    m_txtPreview->setObjectName(QSL("m_txtPreview"));
    m_txtPreview->setOpenExternalLinks(false);
    m_txtPreview->setOpenLinks(false);

    QWidget* widget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(widget);
    QToolBar* tool_bar = new QToolBar(widget);

    tool_bar->addAction(m_actionRefreshPreview);
    tool_bar->setIconSize(QSize(16, 16));
    layout->setMargin(0);
    layout->addWidget(tool_bar);
    layout->addWidget(m_txtPreview, 1);

    setWidget(widget);
  }
}

QString MarkdownSidebar::convertMarkdownToHtml(const uint8_t* raw_utf8_data) {
  size_t delka = strlen((const char*)raw_utf8_data);

  if (delka <= 0) {
    return QString();
  }
  else {
    hoedown_renderer* renderer = hoedown_html_renderer_new(hoedown_html_flags::HOEDOWN_HTML_USE_XHTML, 0);
    hoedown_document* document = hoedown_document_new(renderer, hoedown_extensions::HOEDOWN_EXT_MATH, 16);
    hoedown_buffer* html = hoedown_buffer_new(delka);

    // We render Markdown into buffer.
    hoedown_document_render(document, html, raw_utf8_data, delka);

    QString arr = QString::fromUtf8((const char*)html->data, int(html->size));

    hoedown_buffer_free(html);
    hoedown_document_free(document);
    hoedown_html_renderer_free(renderer);

    return arr;
  }
}
