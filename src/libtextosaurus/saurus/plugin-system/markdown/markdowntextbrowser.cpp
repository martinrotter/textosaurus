// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "saurus/plugin-system/markdown/markdowntextbrowser.h"

#include "common/miscellaneous/iofactory.h"

#include <QDir>
#include <QFile>

MarkdownTextBrowser::MarkdownTextBrowser(QWidget* parent) : QTextBrowser(parent) {}

QVariant MarkdownTextBrowser::loadResource(int type, const QUrl& name) {
  /*if (type == QTextDocument::ResourceType::ImageResource) {
     QString local_file = name.toString();

     if (local_file.startsWith(QL1S(":/"))) {
      local_file = local_file.mid(2);
     }

     if (QFile::exists(local_file)) {
      return IOFactory::readFile(local_file);
     }

     QString absolute_file = m_documentBaseFolder + QDir::separator() + local_file;

     if (QFile::exists(absolute_file)) {
      return IOFactory::readFile(absolute_file);
     }
     }*/

  return QTextBrowser::loadResource(type, name);
}

void MarkdownTextBrowser::setMarkdownDocument(const QString& base_folder, const QString& html_contents) {
  m_documentBaseFolder = base_folder;
  setText(html_contents);
}

void MarkdownTextBrowser::clearMarkdownDocument() {
  m_documentBaseFolder = QString();
  clear();
}
