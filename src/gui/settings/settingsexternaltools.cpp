// For license of this file, see <project-root-folder>/LICENSE.md.

#include "gui/settings/settingsexternaltools.h"

#include "external-tools/externaltool.h"
#include "external-tools/externaltools.h"
#include "miscellaneous/application.h"
#include "miscellaneous/iconfactory.h"
#include "miscellaneous/textapplication.h"
#include "miscellaneous/textapplicationsettings.h"

SettingsExternalTools::SettingsExternalTools(Settings* settings, QWidget* parent)
  : SettingsPanel(settings, parent) {
  m_ui.setupUi(this);
  m_ui.m_shortcut->hideResetButton();
  m_ui.m_btnAdd->setIcon(qApp->icons()->fromTheme(QSL("list-add")));
  m_ui.m_btnDelete->setIcon(qApp->icons()->fromTheme(QSL("list-remove")));

  m_ui.m_cmbInput->addItem(tr("Current selection/document"), int(ToolInput::SelectionDocument));
  m_ui.m_cmbInput->addItem(tr("Current line"), int(ToolInput::CurrentLine));
  m_ui.m_cmbInput->addItem(tr("Saved file path"), int(ToolInput::SavedFile));
  m_ui.m_cmbInput->addItem(tr("No input"), int(ToolInput::NoInput));

  m_ui.m_cmbOutput->addItem(tr("Replace current selection/document"), int(ToolOutput::ReplaceSelectionDocument));
  m_ui.m_cmbOutput->addItem(tr("Insert at cursor position"), int(ToolOutput::InsertAtCursorPosition));
  m_ui.m_cmbOutput->addItem(tr("Dump to output window"), int(ToolOutput::DumpToOutputWindow));
  m_ui.m_cmbOutput->addItem(tr("Copy to clipboard"), int(ToolOutput::CopyToClipboard));
  m_ui.m_cmbOutput->addItem(tr("New file"), int(ToolOutput::NewSavedFile));
  m_ui.m_cmbOutput->addItem(tr("No output"), int(ToolOutput::NoOutput));

  connect(m_ui.m_listTools, &QListWidget::currentItemChanged, this, &SettingsExternalTools::displayToolDetails);
  connect(m_ui.m_btnAdd, &PlainToolButton::clicked, this, &SettingsExternalTools::addNewTool);
  connect(m_ui.m_btnDelete, &PlainToolButton::clicked, this, &SettingsExternalTools::removeSelectedTool);
  connect(m_ui.m_txtTitle, &QLineEdit::textEdited, this, &SettingsExternalTools::updateToolListNames);

  /*
     connect(m_ui.m_txtFullScript, &QPlainTextEdit::textChanged, this, &SettingsExternalTools::saveCurrentTool);
     connect(m_ui.m_txtSimpleCommand, &QLineEdit::textEdited, this, &SettingsExternalTools::saveCurrentTool);
     connect(m_ui.m_txtTitle, &QLineEdit::textEdited, this, &SettingsExternalTools::updateToolListNames);
     connect(m_ui.m_txtTitle, &QLineEdit::textEdited, this, &SettingsExternalTools::saveCurrentTool);
     connect(m_ui.m_txtCategory, &QLineEdit::textEdited, this, &SettingsExternalTools::saveCurrentTool);
     connect(m_ui.m_toolBoxScript, &QToolBox::currentChanged, this, &SettingsExternalTools::saveCurrentTool);
     connect(m_ui.m_cmbInput,
          static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
          this,
          &SettingsExternalTools::saveCurrentTool);
     connect(m_ui.m_cmbOutput,
          static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
          this,
          &SettingsExternalTools::saveCurrentTool);
     connect(m_ui.m_shortcut, &ShortcutCatcher::shortcutChanged, this, &SettingsExternalTools::saveCurrentTool);
   */

  displayToolDetails(nullptr, nullptr);
}

void SettingsExternalTools::loadSettings() {
  onBeginLoadSettings();

  const QList<ExternalTool*> tools = qApp->textApplication()->settings()->externalTools()->tools();

  foreach (const ExternalTool* tool, tools) {
    if (!tool->isPredefined()) {
      // We create clone of our tool.
      ExternalTool* work_tool = new ExternalTool(*tool, this);
      QListWidgetItem* work_item = new QListWidgetItem(work_tool->name(), m_ui.m_listTools);

      work_item->setData(Qt::UserRole, QVariant::fromValue(work_tool));
      m_ui.m_listTools->addItem(work_item);
    }
  }

  onEndLoadSettings();
}

void SettingsExternalTools::saveSettings() {
  onBeginSaveSettings();

  // For sure, save first.
  saveCurrentTool();

  onEndSaveSettings();
}

void SettingsExternalTools::addNewTool() {
  QListWidgetItem* item = new QListWidgetItem(tr("New tool"), m_ui.m_listTools);
  ExternalTool* ext_tool = new ExternalTool(this);

  ext_tool->setName(tr("New tool"));
  item->setData(Qt::UserRole, QVariant::fromValue(ext_tool));

  m_ui.m_listTools->addItem(item);
}

void SettingsExternalTools::removeSelectedTool() {}

void SettingsExternalTools::saveCurrentTool() {
  saveToolChanges(m_ui.m_listTools->currentItem());
}

void SettingsExternalTools::updateToolListNames(const QString& name) {
  if (m_ui.m_listTools->currentItem() != nullptr) {
    m_ui.m_listTools->currentItem()->setText(name);
  }
}

void SettingsExternalTools::saveToolChanges(QListWidgetItem* item) {
  if (item != nullptr) {
    ExternalTool* ext_tool = item->data(Qt::UserRole).value<ExternalTool*>();

    ext_tool->setScript(m_ui.m_txtFullScript->toPlainText());
    ext_tool->setName(m_ui.m_txtTitle->text());
    ext_tool->setCategory(m_ui.m_txtCategory->text());
    ext_tool->setInput(ToolInput(m_ui.m_cmbInput->currentData().toInt()));
    ext_tool->setOutput(ToolOutput(m_ui.m_cmbOutput->currentData().toInt()));
    ext_tool->setShortcut(m_ui.m_shortcut->shortcut().toString(QKeySequence::SequenceFormat::PortableText));
  }
}

void SettingsExternalTools::displayToolDetails(QListWidgetItem* current, QListWidgetItem* previous) {
  m_ui.m_btnDelete->setEnabled(current != nullptr);

  // We save previous tool first.
  if (previous != nullptr) {
    saveToolChanges(previous);
  }

  if (current != nullptr) {
    m_ui.m_toolDetails->setEnabled(true);

    // We display new tool.
    ExternalTool* new_tool = current->data(Qt::UserRole).value<ExternalTool*>();

    m_ui.m_txtTitle->setText(new_tool->name());
    m_ui.m_txtCategory->setText(new_tool->category());

    m_ui.m_shortcut->blockSignals(true);
    m_ui.m_shortcut->setShortcut(QKeySequence::fromString(new_tool->shortcut(), QKeySequence::SequenceFormat::PortableText));
    m_ui.m_shortcut->blockSignals(false);

    m_ui.m_cmbInput->setCurrentIndex(m_ui.m_cmbInput->findData(int(new_tool->input())));
    m_ui.m_cmbOutput->setCurrentIndex(m_ui.m_cmbOutput->findData(int(new_tool->output())));
    m_ui.m_txtFullScript->setPlainText(new_tool->script());
  }
  else {
    m_ui.m_toolDetails->setEnabled(false);
  }
}
