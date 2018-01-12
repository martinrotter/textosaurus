// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef TOOLBOX_H
#define TOOLBOX_H

#include "gui/sidebars/dockwidget.h"

#include <QMessageBox>
#include <QUrl>

#include <functional>

class QTextBrowser;

enum class OutputSource {
  ExternalTool,
  Application
};

class OutputSidebar : public DockWidget {
  Q_OBJECT

  public:
    explicit OutputSidebar(TextApplication* text_app, QWidget* parent = nullptr);
    virtual ~OutputSidebar() = default;

    virtual Qt::DockWidgetArea initialArea() const override;
    virtual bool initiallyVisible() const override;
    virtual int initialWidth() const override;

  public slots:
    void displayOutput(OutputSource source, const QString& message, QMessageBox::Icon level = QMessageBox::Icon::NoIcon,
                       const QUrl& url = QUrl(), std::function<void()> handler = {});

  private:
    virtual void load() override;

    QColor colorForLevel(QMessageBox::Icon level);

  private:
    QHash<QUrl, std::function<void()>> m_handlers;
    QMessageBox::Icon m_currentLevel;
    QTextBrowser* m_txtOutput;
};

#endif // TOOLBOX_H
