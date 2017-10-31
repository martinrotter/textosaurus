// For license of this file, see <project-root-folder>/LICENSE.md.

#include "gui/toolbox.h"

#include "gui/plaintoolbutton.h"
#include "miscellaneous/application.h"
#include "miscellaneous/iconfactory.h"

#include <QPlainTextEdit>
#include <QScrollBar>

ToolBox::ToolBox(QWidget* parent) : QTabWidget(parent), m_txtOutput(new QPlainTextEdit(this)) {
  PlainToolButton* btn_close = new PlainToolButton(this);

  btn_close->setIcon(qApp->icons()->fromTheme(QSL("window-close")));
  btn_close->setToolTip("Hide toolbox");

  connect(btn_close, &PlainToolButton::clicked, this, &ToolBox::hide);

  setCornerWidget(btn_close, Qt::BottomLeftCorner);
  addTab(m_txtOutput, qApp->icons()->fromTheme(QSL("application-text")), tr("Output"));
  setTabPosition(QTabWidget::TabPosition::South);

  //setVisible(false);
}

void ToolBox::displayOutput(const QString& source, const QString& message) {
  m_txtOutput->appendPlainText(QString("%1: %2\n").arg(source, message));
  m_txtOutput->verticalScrollBar()->setValue(m_txtOutput->verticalScrollBar()->maximum());
  setVisible(true);
}
