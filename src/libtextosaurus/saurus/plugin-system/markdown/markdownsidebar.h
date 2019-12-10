// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifndef MARKDOWNSIDEBAR_H
#define MARKDOWNSIDEBAR_H

#include "saurus/gui/sidebars/basesidebar.h"

class TextApplication;
class MarkdownPlugin;
class MarkdownTextBrowser;

class MarkdownSidebar : public BaseSidebar {
  Q_OBJECT

  friend class MarkdownPlugin;

  public:
    explicit MarkdownSidebar(MarkdownPlugin* plugin, QWidget* parent = nullptr);

    virtual Qt::DockWidgetArea initialArea() const override;
    virtual bool initiallyVisible() const override;
    virtual int initialWidth() const override;

  protected:
    virtual void load() override;

  private slots:
    void refreshPreview();

  private:
    QString convertMarkdownToHtml(const uint8_t* raw_utf8_data);

  private:
    MarkdownTextBrowser* m_txtPreview;
    MarkdownPlugin* m_plugin;
    QAction* m_actionRefreshPreview;
};

#endif // MARKDOWNSIDEBAR_H
