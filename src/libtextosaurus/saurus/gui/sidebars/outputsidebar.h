// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifndef TOOLBOX_H
#define TOOLBOX_H

#include "saurus/gui/sidebars/basesidebar.h"

#include <QMessageBox>
#include <QPointer>
#include <QUrl>

#include <functional>

class QTextBrowser;

enum class OutputSource {
  ExternalTool,
  Application
};

class OutputSidebar : public BaseSidebar {
  Q_OBJECT

  public:
    explicit OutputSidebar(TextApplication* text_app, QWidget* parent = nullptr);
    virtual ~OutputSidebar() = default;

    virtual Qt::DockWidgetArea initialArea() const override;
    virtual bool initiallyVisible() const override;
    virtual int initialWidth() const override;

  public slots:
    void displayOutput(OutputSource source, const QString& message, QMessageBox::Icon level = QMessageBox::Icon::NoIcon,
                       const QUrl& url = QUrl(), QObject* target = nullptr);

  private:
    virtual void load() override;

    QColor colorForLevel(QMessageBox::Icon level);

  private:
    QHash<QUrl, QPointer<QObject>> m_handlers;
    QMessageBox::Icon m_currentLevel;
    QTextBrowser* m_txtOutput;
};

#endif // TOOLBOX_H
