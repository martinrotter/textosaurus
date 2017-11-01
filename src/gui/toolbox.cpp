// For license of this file, see <project-root-folder>/LICENSE.md.

#include "gui/toolbox.h"

#include "gui/plaintoolbutton.h"
#include "miscellaneous/application.h"
#include "miscellaneous/iconfactory.h"

#include <QLayout>
#include <QPlainTextEdit>
#include <QScrollBar>
#include <QTabBar>

ToolBox::ToolBox(QWidget* parent) : QTabWidget(parent), m_txtOutput(new QPlainTextEdit(this)) {
  PlainToolButton* btn_close = new PlainToolButton(this);

  btn_close->setIcon(qApp->icons()->fromTheme(QSL("window-close")));
  btn_close->setToolTip("Hide toolbox");

  connect(btn_close, &PlainToolButton::clicked, this, &ToolBox::hide);

  setTabPosition(QTabWidget::TabPosition::South);
  setCornerWidget(btn_close, Qt::BottomLeftCorner);
  setVisible(false);
  setContentsMargins(0, 0, 0, 0);

  // Tweak corner button size/position.
  int available_height = tabBar()->height();
  QSize optimal_size = btn_close->sizeHint();
  int btn_side = qMin(optimal_size.width(), available_height) - 4;
  int ideal_side = qAbs(btn_side - available_height) / 2;

  btn_close->setMaximumSize(btn_side, btn_side);
  btn_close->setPadding(ideal_side, ideal_side - 1, 0, 0);

  m_txtOutput->setPlaceholderText(tr("This window can display output of external tools and some other critical information..."));
  m_txtOutput->setFocusPolicy(Qt::FocusPolicy::ClickFocus);
  m_txtOutput->setWordWrapMode(QTextOption::WrapMode::WrapAnywhere);
  m_txtOutput->setReadOnly(true);
  m_txtOutput->setFont(QFontDatabase::systemFont(QFontDatabase::SystemFont::FixedFont));
  addTab(m_txtOutput, qApp->icons()->fromTheme(QSL("application-text")), tr("Output"));
}

void ToolBox::displayOutput(OutputSource source, const QString& message) {
  setCurrentWidget(m_txtOutput);
  setVisible(true);
  m_txtOutput->appendPlainText(QString("[%3] %1: %2\n").arg(descriptionOfSource(source),
                                                            message,
                                                            QDateTime::currentDateTime().toString(QSL(FORMAT_DATETIME_OUTPUT))));
  m_txtOutput->verticalScrollBar()->setValue(m_txtOutput->verticalScrollBar()->maximum());
}

QString ToolBox::descriptionOfSource(OutputSource source) {
  switch (source) {
    case OutputSource::Application:
      return tr("application");

    case OutputSource::ExternalTool:
      return tr("external tool");

    case OutputSource::TextApplication:
      return tr("editor");

    default:
      return QString();
  }
}
