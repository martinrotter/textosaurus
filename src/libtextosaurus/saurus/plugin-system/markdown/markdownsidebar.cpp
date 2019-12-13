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

#include "3rd-party/md4c/md4c.h"
#include "3rd-party/md4c/render_html.h"

#include <QTextBrowser>
#include <QToolBar>
#include <QVBoxLayout>

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
    QByteArray dat = editor->getText(editor->length() + 1);

    m_txtPreview->setMarkdownDocument(QFileInfo(editor->filePath()).absolutePath(),
                                      convertMarkdownToHtml(dat.data()));
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

QString MarkdownSidebar::convertMarkdownToHtml(char* raw_data) {
  size_t data_len = strlen(raw_data);

  if (data_len <= 0) {
    return QString();
  }
  else {
    QByteArray array;
    int render_result = md_render_html(raw_data,
                                       MD_SIZE(data_len),
                                       &MarkdownSidebar::captureHtmlFragment,
                                       &array,
                                       MD_DIALECT_GITHUB | MD_FLAG_WIKILINKS | MD_FLAG_LATEXMATHSPANS | MD_FLAG_PERMISSIVEATXHEADERS,
                                       0);

    if (render_result == 0) {
      return QString::fromUtf8(array);
    }
    else {
      return QString();
    }
  }
}

void MarkdownSidebar::captureHtmlFragment(const MD_CHAR* data, MD_SIZE data_size, void* userData) {
  QByteArray* array = static_cast<QByteArray*>(userData);

  if (data_size > 0) {
    array->append(data, int(data_size));
  }
}
