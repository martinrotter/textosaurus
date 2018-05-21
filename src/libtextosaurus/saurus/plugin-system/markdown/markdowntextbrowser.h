// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifndef MARKDOWNTEXTBROWSER_H
#define MARKDOWNTEXTBROWSER_H

#include <QTextBrowser>

class MarkdownTextBrowser : public QTextBrowser {
  Q_OBJECT

  public:
    explicit MarkdownTextBrowser(QWidget* parent = nullptr);

    virtual QVariant loadResource(int type, const QUrl& name) override;

  public slots:
    void setMarkdownDocument(const QString& base_folder, const QString& html_contents);
    void clearMarkdownDocument();

  private:
    QString m_documentBaseFolder;
};

#endif // MARKDOWNTEXTBROWSER_H
