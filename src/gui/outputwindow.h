// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef TOOLBOX_H
#define TOOLBOX_H

#include <QDockWidget>

#include <QMessageBox>

class QPlainTextEdit;

enum class OutputSource {
  TextApplication,
  ExternalTool,
  Application
};

class OutputWindow : public QDockWidget {
  Q_OBJECT

  public:
    explicit OutputWindow(QWidget* parent = nullptr);

  public slots:
    void displayOutput(OutputSource source, const QString& message, QMessageBox::Icon level = QMessageBox::Icon::NoIcon);

  private:
    QMessageBox::Icon m_currentLevel;
    QPlainTextEdit* m_txtOutput;

    QColor colorForLevel(QMessageBox::Icon level);
};

#endif // TOOLBOX_H
