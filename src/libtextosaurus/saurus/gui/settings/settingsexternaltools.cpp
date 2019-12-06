// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "saurus/gui/settings/settingsexternaltools.h"

#include "common/gui/guiutilities.h"
#include "common/miscellaneous/iconfactory.h"
#include "saurus/external-tools/externaltool.h"
#include "saurus/external-tools/externaltools.h"
#include "saurus/miscellaneous/application.h"
#include "saurus/miscellaneous/textapplication.h"
#include "saurus/miscellaneous/textapplicationsettings.h"

SettingsExternalTools::SettingsExternalTools(Settings* settings, QWidget* parent)
  : SettingsPanel(settings, parent), m_isSwitchingSelectedTool(false) {
  m_ui.setupUi(this);
  m_ui.m_shortcut->hideResetButton();
  m_ui.m_txtFullScript->setFont(QFontDatabase::systemFont(QFontDatabase::SystemFont::FixedFont));
  m_ui.m_btnAdd->setIcon(qApp->icons()->fromTheme(QSL("list-add")));
  m_ui.m_btnDelete->setIcon(qApp->icons()->fromTheme(QSL("list-remove")));

  m_ui.m_cmbInput->addItem(tr("Current selection/document"), int(ExternalTool::ToolInput::SelectionDocument));
  m_ui.m_cmbInput->addItem(tr("Current line"), int(ExternalTool::ToolInput::CurrentLine));
  m_ui.m_cmbInput->addItem(tr("Saved file path"), int(ExternalTool::ToolInput::SavedFile));
  m_ui.m_cmbInput->addItem(tr("Ask user for input"), int(ExternalTool::ToolInput::AskForInput));
  m_ui.m_cmbInput->addItem(tr("No input"), int(ExternalTool::ToolInput::NoInput));

  m_ui.m_cmbOutput->addItem(tr("Replace current selection/document"), int(ExternalTool::ToolOutput::ReplaceSelectionDocument));
  m_ui.m_cmbOutput->addItem(tr("Replace current line"), int(ExternalTool::ToolOutput::ReplaceCurrentLine));
  m_ui.m_cmbOutput->addItem(tr("Insert at cursor position"), int(ExternalTool::ToolOutput::InsertAtCursorPosition));
  m_ui.m_cmbOutput->addItem(tr("Insert at cursor position and replace selection (if any)"),
                            int(ExternalTool::ToolOutput::InsertAtCursorPositionAndReplaceSelection));
  m_ui.m_cmbOutput->addItem(tr("Dump to output window"), int(ExternalTool::ToolOutput::DumpToOutputWindow));
  m_ui.m_cmbOutput->addItem(tr("Copy to clipboard"), int(ExternalTool::ToolOutput::CopyToClipboard));
  m_ui.m_cmbOutput->addItem(tr("New file"), int(ExternalTool::ToolOutput::NewSavedFile));
  m_ui.m_cmbOutput->addItem(tr("Reload file"), int(ExternalTool::ToolOutput::ReloadFile));
  m_ui.m_cmbOutput->addItem(tr("No output"), int(ExternalTool::ToolOutput::NoOutput));

  connect(m_ui.m_listTools, &QListWidget::currentItemChanged, this, &SettingsExternalTools::displayToolDetails);
  connect(m_ui.m_btnAdd, &PlainToolButton::clicked, this, &SettingsExternalTools::addNewTool);
  connect(m_ui.m_btnDelete, &PlainToolButton::clicked, this, &SettingsExternalTools::removeSelectedTool);
  connect(m_ui.m_txtTitle, &QLineEdit::textEdited, this, &SettingsExternalTools::updateToolListNames);

  connect(m_ui.m_txtFullScript, &QPlainTextEdit::textChanged, this, &SettingsExternalTools::dirtifySettings);
  connect(m_ui.m_txtInterpreter, &QLineEdit::textEdited, this, &SettingsExternalTools::dirtifySettings);
  connect(m_ui.m_txtTitle, &QLineEdit::textEdited, this, &SettingsExternalTools::dirtifySettings);
  connect(m_ui.m_txtPrompt, &QLineEdit::textEdited, this, &SettingsExternalTools::dirtifySettings);
  connect(m_ui.m_txtTitle, &QLineEdit::textEdited, this, &SettingsExternalTools::dirtifySettings);
  connect(m_ui.m_txtCategory, &QLineEdit::textEdited, this, &SettingsExternalTools::dirtifySettings);
  connect(m_ui.m_cmbInput,
          static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
          this,
          &SettingsExternalTools::dirtifySettings);
  connect(m_ui.m_cmbOutput,
          static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
          this,
          &SettingsExternalTools::dirtifySettings);
  connect(m_ui.m_shortcut, &ShortcutCatcher::shortcutChanged, this, &SettingsExternalTools::dirtifySettings);

  GuiUtilities::setLabelAsNotice(*m_ui.m_lblInfo, true);
  displayToolDetails(nullptr, nullptr);
}

void SettingsExternalTools::loadSettings() {
  onBeginLoadSettings();

  auto tools = qApp->textApplication()->settings()->externalTools()->customTools();

  foreach (const ExternalTool* tool, tools) {
    if (!tool->isPredefined()) {
      // We create clone of our tool.
      auto* work_item = new QListWidgetItem(tool->name(), m_ui.m_listTools);
      auto* work_tool = new ExternalTool(*tool, this);

      work_item->setData(Qt::UserRole, QVariant::fromValue(work_tool));
      m_ui.m_listTools->addItem(work_item);
    }
  }

  onEndLoadSettings();
}

void SettingsExternalTools::saveSettings() {
  onBeginSaveSettings();

  m_isSwitchingSelectedTool = true;
  saveCurrentTool();
  m_isSwitchingSelectedTool = false;

  // Now we save our custom tools.
  QList<ExternalTool*> custom_tools;

  for (int i = 0; i < m_ui.m_listTools->count(); i++) {
    auto* tool = m_ui.m_listTools->item(i)->data(Qt::UserRole).value<ExternalTool*>();

    if (!tool->isPredefined()) {
      custom_tools.append(tool);
    }
  }

  // We save/activate new custom tools.
  qApp->textApplication()->settings()->externalTools()->saveExternalTools(custom_tools);
  qApp->textApplication()->settings()->externalTools()->reloadTools();

  onEndSaveSettings();
}

void SettingsExternalTools::addNewTool() {
  auto* item = new QListWidgetItem(tr("New tool"), m_ui.m_listTools);
  auto* ext_tool = new ExternalTool(this);

  ext_tool->setName(item->text());
  item->setData(Qt::UserRole, QVariant::fromValue(ext_tool));

  m_ui.m_listTools->addItem(item);

  dirtifySettings();
}

void SettingsExternalTools::removeSelectedTool() {
  if (m_ui.m_listTools->currentItem() != nullptr) {
    auto* item = m_ui.m_listTools->currentItem();
    auto* ext_tool = item->data(Qt::UserRole).value<ExternalTool*>();

    ext_tool->deleteLater();
    delete m_ui.m_listTools->takeItem(m_ui.m_listTools->currentRow());

    dirtifySettings();
  }
}

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
    auto* ext_tool = item->data(Qt::UserRole).value<ExternalTool*>();

    ext_tool->setInterpreter(m_ui.m_txtInterpreter->text());
    ext_tool->setScript(m_ui.m_txtFullScript->toPlainText());
    ext_tool->setName(m_ui.m_txtTitle->text());
    ext_tool->setCategory(m_ui.m_txtCategory->text());
    ext_tool->setInput(ExternalTool::ToolInput(m_ui.m_cmbInput->currentData().toInt()));
    ext_tool->setOutput(ExternalTool::ToolOutput(m_ui.m_cmbOutput->currentData().toInt()));
    ext_tool->setPrompt(m_ui.m_txtPrompt->text());
    ext_tool->setShortcut(m_ui.m_shortcut->shortcut().toString(QKeySequence::SequenceFormat::PortableText));
  }
}

void SettingsExternalTools::displayToolDetails(QListWidgetItem* current, QListWidgetItem* previous) {
  m_ui.m_btnDelete->setEnabled(current != nullptr);

  m_isSwitchingSelectedTool = true;

  // We save previous tool first.
  if (previous != nullptr) {
    saveToolChanges(previous);
  }

  if (current != nullptr) {
    m_ui.m_toolDetails->setEnabled(true);

    // We display new tool.
    auto* new_tool = current->data(Qt::UserRole).value<ExternalTool*>();

    m_ui.m_txtInterpreter->setText(new_tool->interpreter());
    m_ui.m_txtTitle->setText(new_tool->name());
    m_ui.m_txtCategory->setText(new_tool->category());
    m_ui.m_txtPrompt->setText(new_tool->prompt());
    m_ui.m_shortcut->setShortcut(QKeySequence::fromString(new_tool->shortcut(), QKeySequence::SequenceFormat::PortableText));
    m_ui.m_cmbInput->setCurrentIndex(m_ui.m_cmbInput->findData(int(new_tool->input())));
    m_ui.m_cmbOutput->setCurrentIndex(m_ui.m_cmbOutput->findData(int(new_tool->output())));
    m_ui.m_txtFullScript->setPlainText(new_tool->script());
  }
  else {
    m_ui.m_toolDetails->setEnabled(false);

    m_ui.m_txtInterpreter->clear();
    m_ui.m_txtTitle->clear();
    m_ui.m_txtCategory->clear();
    m_ui.m_txtPrompt->clear();
    m_ui.m_shortcut->clearShortcut();
    m_ui.m_cmbInput->setCurrentIndex(0);
    m_ui.m_cmbOutput->setCurrentIndex(0);
    m_ui.m_txtFullScript->clear();
  }

  m_isSwitchingSelectedTool = false;
}

void SettingsExternalTools::dirtifySettings() {
  if (!m_isSwitchingSelectedTool) {
    SettingsPanel::dirtifySettings();
  }
}
