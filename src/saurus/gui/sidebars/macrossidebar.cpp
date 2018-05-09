// For license of this file, see <project-root-folder>/LICENSE.md.

#include "saurus/gui/sidebars/macrossidebar.h"

#include "common/miscellaneous/iconfactory.h"
#include "saurus/gui/sidebars/macroswidget.h"
#include "saurus/miscellaneous/application.h"
#include "saurus/miscellaneous/textapplication.h"

#include <QToolBar>

MacrosSidebar::MacrosSidebar(TextApplication* app, QWidget* parent) : BaseSidebar(app, parent) {
  setObjectName(QSL("m_macrosSidebar"));
  setWindowTitle(tr("Macros"));
}

MacrosSidebar::~MacrosSidebar() {}

Qt::DockWidgetArea MacrosSidebar::initialArea() const {
  return Qt::DockWidgetArea::RightDockWidgetArea;
}

bool MacrosSidebar::initiallyVisible() const {
  return false;
}

int MacrosSidebar::initialWidth() const {
  return 250;
}

void MacrosSidebar::load() {
  if (m_widget == nullptr) {
    m_widget = new MacrosWidget(this);

    // Setup icons.
    auto tool_bar = new QToolBar(tr("Macros Toolbar"), m_widget);

    tool_bar->setFixedHeight(26);
    tool_bar->setIconSize(QSize(16, 16));

    m_actionRecordStart = tool_bar->addAction(qApp->icons()->fromTheme(QSL("gtk-media-record")), tr("Start Recording"));
    m_actionRecordStop = tool_bar->addAction(qApp->icons()->fromTheme(QSL("gtk-media-stop")), tr("Stop Recording"));
    m_actionPlay = tool_bar->addAction(qApp->icons()->fromTheme(QSL("media-playback-start")), tr("Play Recorded Macro"));
    m_actionSave = tool_bar->addAction(qApp->icons()->fromTheme(QSL("document-save-as")), tr("Save Macro as..."));
    m_widget->m_ui.verticalLayout->insertWidget(0, tool_bar, 1);

    setWidget(m_widget);
  }
}
