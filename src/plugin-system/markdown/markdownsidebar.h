// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef MARKDOWNSIDEBAR_H
#define MARKDOWNSIDEBAR_H

#include "gui/sidebars/basesidebar.h"

class TextApplication;
class MarkdownPlugin;
class QTextBrowser;

class MarkdownSidebar : public BaseSidebar {
  Q_OBJECT

  friend class MarkdownPlugin;

  public:
    explicit MarkdownSidebar(TextApplication* text_app, QWidget* parent = nullptr);

    virtual Qt::DockWidgetArea initialArea() const override;
    virtual bool initiallyVisible() const override;
    virtual int initialWidth() const override;

  public slots:
    void refreshPreview();

    virtual void load() override;

  private:
    QString convertMarkdownToHtml(const uint8_t* raw_utf8_data);

  private:
    QTextBrowser* m_txtPreview;
    QAction* m_actionRefreshPreview;
};

#endif // MARKDOWNSIDEBAR_H
