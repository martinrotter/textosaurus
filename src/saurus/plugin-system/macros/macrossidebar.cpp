// For license of this file, see <project-root-folder>/LICENSE.md.

#include "saurus/plugin-system/macros/macrossidebar.h"

#include "common/miscellaneous/iconfactory.h"
#include "saurus/gui/tabwidget.h"
#include "saurus/gui/texteditor.h"
#include "saurus/miscellaneous/application.h"
#include "saurus/miscellaneous/textapplication.h"
#include "saurus/plugin-system/macros/macro.h"
#include "saurus/plugin-system/macros/macros.h"
#include "saurus/plugin-system/macros/macroswidget.h"

#include <QMetaEnum>
#include <QToolBar>

MacrosSidebar::MacrosSidebar(TextApplication* text_app, Macros* macros_factory, QWidget* parent)
  : BaseSidebar(text_app, parent), m_macrosFactory(macros_factory) {
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

    connect(m_actionPlay, &QAction::triggered, this, &MacrosSidebar::playMacro);
    connect(m_actionSave, &QAction::triggered, this, &MacrosSidebar::saveMacroAs);
    connect(m_actionRecordStart, &QAction::triggered, this, &MacrosSidebar::startRecording);
    connect(m_actionRecordStop, &QAction::triggered, this, &MacrosSidebar::stopRecording);
    connect(m_macrosFactory, &Macros::newStepRecorded, this, &MacrosSidebar::loadNewRecordedMacroStep);

    m_actionRecordStart->setEnabled(true);
    m_actionRecordStop->setEnabled(false);
    m_actionPlay->setEnabled(false);
    m_actionSave->setEnabled(false);
  }
}

void MacrosSidebar::startRecording() {
  m_actionRecordStart->setEnabled(false);
  m_actionRecordStop->setEnabled(true);
  m_actionPlay->setEnabled(false);
  m_actionSave->setEnabled(false);
  m_widget->m_ui.m_listSteps->clear();

  m_macrosFactory->recordNewMacro(m_textApp->tabWidget()->currentEditor());
}

void MacrosSidebar::stopRecording() {
  m_actionRecordStart->setEnabled(true);
  m_actionRecordStop->setEnabled(false);
  m_actionPlay->setEnabled(true);
  m_actionSave->setEnabled(true);

  m_macrosFactory->stopMacroRecording();
}

void MacrosSidebar::saveMacroAs() {}

void MacrosSidebar::playMacro() {
  m_actionRecordStart->setEnabled(false);
  m_actionRecordStop->setEnabled(false);
  m_actionPlay->setEnabled(false);
  m_actionSave->setEnabled(false);

  // TODO: teď se přehraje makro.
  auto recorded_macro = m_macrosFactory->recordedMacro();

  if (recorded_macro != nullptr) {
    m_macrosFactory->recordedMacro()->play(m_textApp->tabWidget()->currentEditor());
  }

  m_actionRecordStart->setEnabled(true);
  m_actionRecordStop->setEnabled(false);
  m_actionPlay->setEnabled(true);
  m_actionSave->setEnabled(true);
}

void MacrosSidebar::loadNewRecordedMacroStep(Macro::MacroStep step) {
  auto recorded_macro = m_macrosFactory->recordedMacro();

  if (recorded_macro->macroSteps().size() > m_widget->m_ui.m_listSteps->count()) {
    // New macro step.
    m_widget->m_ui.m_listSteps->addItem(QMetaEnum::fromType<Macro::ScintillaCommand>().valueToKey(int(step.m_cmd)));
  }
  else {
    // Macros engine modified last step.
    m_widget->m_ui.m_listSteps->item(m_widget->m_ui.m_listSteps->count() -
                                     1)->setText(QMetaEnum::fromType<Macro::ScintillaCommand>().valueToKey(int(step.m_cmd)));
  }

  m_widget->m_ui.m_listSteps->scrollToBottom();
}
