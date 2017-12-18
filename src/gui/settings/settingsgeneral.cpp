// For license of this file, see <project-root-folder>/LICENSE.md.

#include "gui/settings/settingsgeneral.h"

#include "miscellaneous/application.h"
#include "miscellaneous/systemfactory.h"

SettingsGeneral::SettingsGeneral(Settings* settings, QWidget* parent) : SettingsPanel(settings, parent) {
  m_ui.setupUi(this);
  connect(m_ui.m_checkRemoveTrolltechJunk, &QCheckBox::stateChanged, this, &SettingsGeneral::dirtifySettings);
}

void SettingsGeneral::loadSettings() {
  onBeginLoadSettings();

#if defined(Q_OS_WIN)
  m_ui.m_checkRemoveTrolltechJunk->setVisible(true);
  m_ui.m_checkRemoveTrolltechJunk->setChecked(settings()->value(GROUP(General), SETTING(General::RemoveTrolltechJunk)).toBool());
#else
  m_ui.m_checkRemoveTrolltechJunk->setVisible(false);
#endif
  onEndLoadSettings();
}

void SettingsGeneral::saveSettings() {
  onBeginSaveSettings();
  settings()->setValue(GROUP(General), General::RemoveTrolltechJunk, m_ui.m_checkRemoveTrolltechJunk->isChecked());
  onEndSaveSettings();
}
