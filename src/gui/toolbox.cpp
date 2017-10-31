// For license of this file, see <project-root-folder>/LICENSE.md.

#include "gui/toolbox.h"

#include "miscellaneous/application.h"
#include "miscellaneous/iconfactory.h"

#include <QPlainTextEdit>

ToolBox::ToolBox(QWidget* parent) : QTabWidget(parent), m_txtOutput(new QPlainTextEdit(this)) {
  addTab(m_txtOutput, qApp->icons()->fromTheme(QSL("application-text")), tr("Output"));
  setTabPosition(QTabWidget::TabPosition::South);
  setVisible(false);
}
