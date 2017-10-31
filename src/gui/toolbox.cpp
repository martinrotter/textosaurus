// For license of this file, see <project-root-folder>/LICENSE.md.

#include "gui/toolbox.h"

#include "gui/plaintoolbutton.h"
#include "miscellaneous/application.h"
#include "miscellaneous/iconfactory.h"

#include <QLayout>
#include <QPlainTextEdit>
#include <QScrollBar>

ToolBox::ToolBox(QWidget* parent) : QTabWidget(parent), m_txtOutput(new QPlainTextEdit(this)) {
  PlainToolButton* btn_close = new PlainToolButton(this);

  btn_close->setIcon(qApp->icons()->fromTheme(QSL("window-close")));
  btn_close->setToolTip("Hide toolbox");

  connect(btn_close, &PlainToolButton::clicked, this, &ToolBox::hide);

  setTabPosition(QTabWidget::TabPosition::South);
  setCornerWidget(btn_close, Qt::BottomLeftCorner);

  setVisible(false);
  setContentsMargins(0, 0, 0, 0);

  m_txtOutput->setPlaceholderText(tr("This window can display output of external tools..."));
  m_txtOutput->setFocusPolicy(Qt::FocusPolicy::ClickFocus);
  m_txtOutput->setWordWrapMode(QTextOption::WrapMode::WrapAnywhere);
  m_txtOutput->setReadOnly(true);
  m_txtOutput->setFont(QFontDatabase::systemFont(QFontDatabase::SystemFont::FixedFont));
  addTab(m_txtOutput, qApp->icons()->fromTheme(QSL("application-text")), tr("Output"));
}

void ToolBox::displayOutput(const QString& source, const QString& message) {
  setCurrentWidget(m_txtOutput);
  setVisible(true);
  m_txtOutput->appendPlainText(QString("%1: %2\n").arg(source, message));
  m_txtOutput->verticalScrollBar()->setValue(m_txtOutput->verticalScrollBar()->maximum());
}
