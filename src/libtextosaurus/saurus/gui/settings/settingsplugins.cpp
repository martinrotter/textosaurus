// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "saurus/gui/settings/settingsplugins.h"

#include "common/exceptions/applicationexception.h"
#include "common/network-web/webfactory.h"
#include "saurus/miscellaneous/application.h"
#include "saurus/miscellaneous/textapplication.h"
#include "saurus/miscellaneous/textapplicationsettings.h"
#include "saurus/plugin-system/pluginbase.h"

SettingsPlugins::SettingsPlugins(Settings* settings, QWidget* parent) : SettingsPanel(settings, parent) {
  m_ui.setupUi(this);

  m_ui.m_treePlugins->setColumnCount(3);
  m_ui.m_treePlugins->setHeaderLabels({tr("Name"), tr("Author"), tr("Website")});

  connect(m_ui.m_btnGoToWebsite, &QPushButton::clicked, this, &SettingsPlugins::goToWebsite);
  connect(m_ui.m_treePlugins, &QTreeWidget::currentItemChanged, this, [this](QTreeWidgetItem* current, QTreeWidgetItem* previous) {
    Q_UNUSED(previous)
    m_ui.m_btnGoToWebsite->setEnabled(current != nullptr);
  });

  m_ui.m_btnGoToWebsite->setEnabled(false);
}

QString SettingsPlugins::title() const {
  return tr("Plugins");
}

void SettingsPlugins::loadSettings() {
  onBeginLoadSettings();

  for (PluginState& plugin_state : qApp->textApplication()->settings()->pluginFactory()->plugins()) {
    QTreeWidgetItem* ite = new QTreeWidgetItem();

    ite->setData(0, Qt::ItemDataRole::UserRole, QVariant::fromValue<PluginState>(plugin_state));
    updateRow(ite);

    m_ui.m_treePlugins->addTopLevelItem(ite);
  }

  m_ui.m_treePlugins->resizeColumnToContents(0);
  m_ui.m_treePlugins->resizeColumnToContents(1);

  onEndLoadSettings();
}

void SettingsPlugins::saveSettings() {
  onBeginSaveSettings();

  onEndSaveSettings();
}

void SettingsPlugins::goToWebsite() const {
  qApp->web()->openUrlInExternalBrowser(selectedPlugin().pluginWebsite());
}

void SettingsPlugins::updateRow(QTreeWidgetItem* row) {
  PluginState plugin_state = row->data(0, Qt::ItemDataRole::UserRole).value<PluginState>();

  row->setText(0, plugin_state.pluginName());
  row->setText(1, plugin_state.pluginAuthor());
  row->setText(2, plugin_state.pluginWebsite());
}

PluginState SettingsPlugins::selectedPlugin() const {
  if (m_ui.m_treePlugins->currentItem() != nullptr) {
    return m_ui.m_treePlugins->currentItem()->data(0, Qt::ItemDataRole::UserRole).value<PluginState>();
  }
  else {
    throw ApplicationException(tr("no plugin selected"));
  }
}
