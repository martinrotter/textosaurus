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
    auto tool_bar = new QToolBar(tr("Macro Recorder Toolbar"), m_widget);

    tool_bar->setFixedHeight(26);
    tool_bar->setIconSize(QSize(16, 16));

    m_actionRecordStart = tool_bar->addAction(qApp->icons()->fromTheme(QSL("gtk-media-record")), tr("Start Recording"));
    m_actionRecordStop = tool_bar->addAction(qApp->icons()->fromTheme(QSL("gtk-media-stop")), tr("Stop Recording"));
    m_actionRecordPlay = tool_bar->addAction(qApp->icons()->fromTheme(QSL("media-playback-start")), tr("Play Recorded Macro"));
    m_actionRecordSave = tool_bar->addAction(qApp->icons()->fromTheme(QSL("document-save-as")), tr("Save Macro as..."));
    m_widget->m_ui.verticalLayoutRecorder->insertWidget(0, tool_bar, 1);

    auto tool_bar_stored = new QToolBar(tr("Stored Macros Toolbar"), m_widget);

    tool_bar_stored->setFixedHeight(26);
    tool_bar_stored->setIconSize(QSize(16, 16));

    m_actionStoredPlay = tool_bar_stored->addAction(qApp->icons()->fromTheme(QSL("media-playback-start")), tr("Play Stored Macro"));
    m_actionStoredDelete = tool_bar_stored->addAction(qApp->icons()->fromTheme(QSL("list-remove")), tr("Remove Selected Macro"));
    m_widget->m_ui.verticalLayoutStored->insertWidget(0, tool_bar_stored, 1);

    setWidget(m_widget);

    connect(m_actionRecordPlay, &QAction::triggered, this, &MacrosSidebar::playRecordedMacro);
    connect(m_actionRecordSave, &QAction::triggered, this, &MacrosSidebar::saveRecordedMacroAs);
    connect(m_actionRecordStart, &QAction::triggered, this, &MacrosSidebar::startRecording);
    connect(m_actionRecordStop, &QAction::triggered, this, &MacrosSidebar::stopRecording);
    connect(m_macrosFactory, &Macros::newStepRecorded, this, &MacrosSidebar::loadNewRecordedMacroStep);
    connect(m_widget->m_ui.m_listStoredMacros, &QListWidget::currentRowChanged, this, &MacrosSidebar::onCurrentStoredMacroChanged);
    connect(m_actionStoredPlay, &QAction::triggered, this, &MacrosSidebar::playStoredMacro);
    connect(m_actionStoredDelete, &QAction::triggered, this, &MacrosSidebar::deleteStoredMacro);

    m_actionRecordStart->setEnabled(true);
    m_actionRecordStop->setEnabled(false);
    m_actionRecordPlay->setEnabled(false);
    m_actionRecordSave->setEnabled(false);
    m_actionStoredPlay->setEnabled(false);
    m_actionStoredDelete->setEnabled(false);

    reloadStoredMacros();
  }
}

void MacrosSidebar::startRecording() {
  m_actionRecordStart->setEnabled(false);
  m_actionRecordStop->setEnabled(true);
  m_actionRecordPlay->setEnabled(false);
  m_actionRecordSave->setEnabled(false);
  m_widget->m_ui.m_listRecordedSteps->clear();

  m_macrosFactory->recordNewMacro(m_textApp->tabWidget()->currentEditor());
}

void MacrosSidebar::stopRecording() {
  m_actionRecordStart->setEnabled(true);
  m_actionRecordStop->setEnabled(false);
  m_actionRecordPlay->setEnabled(true);
  m_actionRecordSave->setEnabled(true);

  m_macrosFactory->stopMacroRecording();
}

void MacrosSidebar::saveRecordedMacroAs() {
  if (m_macrosFactory->saveMacroAs(m_macrosFactory->recordedMacro())) {
    reloadStoredMacros();
    m_widget->m_ui.m_tabsMacros->setCurrentIndex(0);
  }
}

void MacrosSidebar::playRecordedMacro() {
  m_actionRecordStart->setEnabled(false);
  m_actionRecordStop->setEnabled(false);
  m_actionRecordPlay->setEnabled(false);
  m_actionRecordSave->setEnabled(false);

  // TODO: teď se přehraje makro.
  auto recorded_macro = m_macrosFactory->recordedMacro();

  if (recorded_macro != nullptr) {
    m_macrosFactory->recordedMacro()->play(m_textApp->tabWidget()->currentEditor());
  }

  m_actionRecordStart->setEnabled(true);
  m_actionRecordStop->setEnabled(false);
  m_actionRecordPlay->setEnabled(true);
  m_actionRecordSave->setEnabled(true);
}

void MacrosSidebar::playStoredMacro() {
  auto it = m_widget->m_ui.m_listStoredMacros->currentItem();

  if (it != nullptr) {
    it->data(Qt::ItemDataRole::UserRole).value<Macro*>()->play(m_textApp->tabWidget()->currentEditor());
  }
}

void MacrosSidebar::deleteStoredMacro() {
  if (m_widget->m_ui.m_listStoredMacros->currentRow() >= 0) {
    m_macrosFactory->deleteMacro(
      m_widget->m_ui.m_listStoredMacros->item(
        m_widget->m_ui.m_listStoredMacros->currentRow())->data(Qt::
                                                               ItemDataRole
                                                               ::UserRole).value<
        Macro*>());
    delete m_widget->m_ui.m_listStoredMacros->takeItem(m_widget->m_ui.m_listStoredMacros->currentRow());
  }
}

void MacrosSidebar::onCurrentStoredMacroChanged(int row) {
  m_actionStoredPlay->setEnabled(row >= 0);
  m_actionStoredDelete->setEnabled(row >= 0);
}

void MacrosSidebar::loadNewRecordedMacroStep(Macro::MacroStep step) {
  auto recorded_macro = m_macrosFactory->recordedMacro();

  if (recorded_macro->macroSteps().size() > m_widget->m_ui.m_listRecordedSteps->count()) {
    // New macro step.
    m_widget->m_ui.m_listRecordedSteps->addItem(QMetaEnum::fromType<Macro::ScintillaCommand>().valueToKey(int(step.m_cmd)));
  }
  else {
    // Macros engine modified last step.
    m_widget->m_ui.m_listRecordedSteps->item(m_widget->m_ui.m_listRecordedSteps->count() -
                                             1)->setText(QMetaEnum::fromType<Macro::ScintillaCommand>().valueToKey(int(step.m_cmd)));
  }

  m_widget->m_ui.m_listRecordedSteps->scrollToBottom();
}

void MacrosSidebar::reloadStoredMacros() {
  m_widget->m_ui.m_listStoredMacros->clear();

  for (Macro* mac : m_macrosFactory->storedMacros()) {
    auto it = new QListWidgetItem(mac->name(), m_widget->m_ui.m_listStoredMacros);

    //m_widget->m_ui.m_listStoredMacros->addItem(it);

    it->setData(Qt::ItemDataRole::UserRole, QVariant::fromValue(mac));
  }
}
