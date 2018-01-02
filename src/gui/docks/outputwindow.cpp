// For license of this file, see <project-root-folder>/LICENSE.md.

#include "gui/docks/outputwindow.h"

#include "gui/plaintoolbutton.h"
#include "miscellaneous/application.h"
#include "miscellaneous/iconfactory.h"

#include <QLayout>
#include <QPlainTextEdit>
#include <QScrollBar>
#include <QTabBar>

OutputWindow::OutputWindow(QWidget* parent) : DockWidget(parent), m_currentLevel(QMessageBox::Icon::Information),
  m_txtOutput(nullptr) {
  setWindowTitle(tr("Output"));
}

void OutputWindow::displayOutput(OutputSource source, const QString& message, QMessageBox::Icon level) {
  Q_UNUSED(source)
  show();
  raise();

  if (level != m_currentLevel) {
    QColor target_color = colorForLevel(level);
    QTextCharFormat c = m_txtOutput->currentCharFormat();

    c.setForeground(target_color);
    m_txtOutput->setCurrentCharFormat(c);
    m_currentLevel = level;
  }

  if (source == OutputSource::ExternalTool) {
    m_txtOutput->appendPlainText(QString("%1").arg(message));
  }
  else {
    m_txtOutput->appendPlainText(QString("[%2] %1").arg(message, QDateTime::currentDateTime().toString(FORMAT_DATETIME_OUTPUT)));
  }
}

QColor OutputWindow::colorForLevel(QMessageBox::Icon level) {
  switch (level) {
    case QMessageBox::Icon::Information:
      return Qt::GlobalColor::blue;

    case QMessageBox::Icon::Warning:
      return QColor(229, 83, 0);

    case QMessageBox::Icon::Critical:
      return Qt::GlobalColor::red;

    default:
      return Qt::GlobalColor::black;
  }
}

Qt::DockWidgetArea OutputWindow::initialArea() const {
  return Qt::DockWidgetArea::BottomDockWidgetArea;
}

bool OutputWindow::initiallyVisible() const {
  return true;
}

int OutputWindow::initialWidth() const {
  return 150;
}

void OutputWindow::load() {
  if (m_txtOutput == nullptr) {
    m_txtOutput = new QPlainTextEdit(this);
    m_txtOutput->setPlaceholderText(tr("This sidebar displays output of external tools and some other critical information..."));
    m_txtOutput->setFocusPolicy(Qt::FocusPolicy::ClickFocus);
    m_txtOutput->setWordWrapMode(QTextOption::WrapMode::WrapAnywhere);
    m_txtOutput->setReadOnly(true);
    m_txtOutput->setObjectName(QSL("m_txtOutput"));
    m_txtOutput->setFont(QFontDatabase::systemFont(QFontDatabase::SystemFont::FixedFont));

    setWidget(m_txtOutput);
  }
}
