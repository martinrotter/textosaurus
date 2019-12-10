// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "saurus/plugin-system/markdown/markdownsidebar.h"

#include "common/miscellaneous/iconfactory.h"
#include "common/network-web/webfactory.h"
#include "saurus/gui/tabwidget.h"
#include "saurus/gui/texteditor.h"
#include "saurus/miscellaneous/application.h"
#include "saurus/miscellaneous/textapplication.h"
#include "saurus/plugin-system/markdown/markdownplugin.h"
#include "saurus/plugin-system/markdown/markdowntextbrowser.h"

#include "3rd-party/hoextdown/hdocument.h"
#include "3rd-party/hoextdown/html.h"

#include <QTextBrowser>
#include <QToolBar>
#include <QVBoxLayout>

using cuint8_ta = const uint8_t *;
using cchara = const char*;

MarkdownSidebar::MarkdownSidebar(MarkdownPlugin* plugin, QWidget* parent)
  : BaseSidebar(plugin->textApp(), parent), m_txtPreview(nullptr), m_plugin(plugin) {
  setWindowTitle(tr("Markdown Preview"));
  setObjectName(QSL("m_sidebarMarkdown"));

  m_actionRefreshPreview = new QAction(m_plugin->iconFactory()->fromTheme(QSL("view-refresh")),
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

  TextEditor* editor = m_textApp->tabWidget()->currentEditor();

  if (editor != nullptr) {
    m_txtPreview->setMarkdownDocument(QFileInfo(editor->filePath()).absolutePath(),
                                      convertMarkdownToHtml(cuint8_ta(editor->characterPointer())));
  }
  else {
    m_txtPreview->clearMarkdownDocument();
  }
}

void MarkdownSidebar::load() {
  if (m_txtPreview == nullptr) {
    m_txtPreview = new MarkdownTextBrowser(this);
    m_txtPreview->setPlaceholderText(tr("This sidebar displays simple HTML preview of your Markdown source..."));
    m_txtPreview->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    m_txtPreview->setWordWrapMode(QTextOption::WrapMode::WrapAnywhere);
    m_txtPreview->setReadOnly(true);
    m_txtPreview->setUndoRedoEnabled(false);
    m_txtPreview->setObjectName(QSL("m_txtPreview"));
    m_txtPreview->setOpenExternalLinks(false);
    m_txtPreview->setOpenLinks(false);

    connect(m_txtPreview, &QTextBrowser::anchorClicked, this, [this](const QUrl& url) {
      m_plugin->webFactory()->openUrlInExternalBrowser(url.toString());
    });

    auto* widget = new QWidget(this);
    auto* layout = new QVBoxLayout(widget);
    auto* tool_bar = new QToolBar(widget);

    tool_bar->addAction(m_actionRefreshPreview);
    tool_bar->setIconSize(QSize(16, 16));

    layout->setMargin(0);
    layout->addWidget(tool_bar);
    layout->addWidget(m_txtPreview, 1);

    setWidget(widget);

    BaseSidebar::load();
  }
}

QString MarkdownSidebar::convertMarkdownToHtml(const uint8_t* raw_utf8_data) {
  size_t delka = strlen(cchara(raw_utf8_data));

  if (delka <= 0) {
    return QString();
  }
  else {
    hoedown_renderer* renderer = hoedown_html_renderer_new(hoedown_html_flags::HOEDOWN_HTML_USE_XHTML, 0);
    hoedown_document* document = hoedown_document_new(renderer,
                                                      hoedown_extensions(4194303), // Enable all flags.
                                                      16,
                                                      0,
                                                      hoedown_user_block(),
                                                      nullptr);
    hoedown_buffer* html = hoedown_buffer_new(delka);

    // We render Markdown into buffer.
    hoedown_document_render(document, html, raw_utf8_data, delka);

    QString arr = QString::fromUtf8(cchara(html->data), int(html->size));

    hoedown_buffer_free(html);
    hoedown_document_free(document);
    hoedown_html_renderer_free(renderer);

    return arr;
  }
}
