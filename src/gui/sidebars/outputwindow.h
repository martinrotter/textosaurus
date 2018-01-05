// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef TOOLBOX_H
#define TOOLBOX_H

#include "gui/sidebars/dockwidget.h"

#include <QMessageBox>

class QPlainTextEdit;

enum class OutputSource {
  ExternalTool,
  Application
};

class OutputWindow : public DockWidget {
  Q_OBJECT

  public:
    explicit OutputWindow(QWidget* parent = nullptr);
    virtual ~OutputWindow() = default;

    virtual Qt::DockWidgetArea initialArea() const override;
    virtual bool initiallyVisible() const override;
    virtual int initialWidth() const override;

  public slots:
    virtual void load() override;

    void displayOutput(OutputSource source, const QString& message, QMessageBox::Icon level = QMessageBox::Icon::NoIcon);

  private:
    QColor colorForLevel(QMessageBox::Icon level);

  private:
    QMessageBox::Icon m_currentLevel;
    QPlainTextEdit* m_txtOutput;
};

#endif // TOOLBOX_H
