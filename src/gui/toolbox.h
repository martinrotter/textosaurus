// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef TOOLBOX_H
#define TOOLBOX_H

#include <QTabWidget>

#include <QMessageBox>

class QPlainTextEdit;

enum class OutputSource {
  TextApplication,
  ExternalTool,
  Application
};

class ToolBox : public QTabWidget {
  Q_OBJECT

  public:
    explicit ToolBox(QWidget* parent = nullptr);

  public slots:
    void displayOutput(OutputSource source, const QString& message, QMessageBox::Icon level = QMessageBox::Icon::NoIcon);

  private:
    QString descriptionOfSource(OutputSource source);

  private:
    QMessageBox::Icon m_currentLevel;
    QPlainTextEdit* m_txtOutput;

    QColor colorForLevel(QMessageBox::Icon level);
};

#endif // TOOLBOX_H
