// For license of this file, see <project-root-folder>/LICENSE.md.

#include "saurus/gui/settings/settingsplugins.h"

#include "saurus/miscellaneous/application.h"
#include "saurus/miscellaneous/textapplication.h"
#include "saurus/miscellaneous/textapplicationsettings.h"
#include "saurus/plugin-system/pluginbase.h"
#include "saurus/plugin-system/pluginfactory.h"

SettingsPlugins::SettingsPlugins(Settings* settings, QWidget* parent) : SettingsPanel(settings, parent) {
  m_ui.setupUi(this);
  m_ui.m_treePlugins->setColumnCount(2);
  m_ui.m_treePlugins->setHeaderLabels({tr("Name"), tr("Author")});
}

QString SettingsPlugins::title() const {
  return tr("Plugins");
}

void SettingsPlugins::loadSettings() {
  onBeginLoadSettings();

  for (PluginState& plugin_state : qApp->textApplication()->settings()->pluginFactory()->plugins()) {
    QTreeWidgetItem* ite = new QTreeWidgetItem();

    ite->setText(0, plugin_state.pluginName());
    ite->setText(1, plugin_state.pluginAuthor());

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
