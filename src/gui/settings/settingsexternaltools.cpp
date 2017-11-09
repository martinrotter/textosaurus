// For license of this file, see <project-root-folder>/LICENSE.md.

#include "gui/settings/settingsexternaltools.h"

SettingsExternalTools::SettingsExternalTools(Settings* settings, QWidget* parent)
  : SettingsPanel(settings, parent) {
  m_ui.setupUi(this);
  m_ui.m_shortcut->hideResetButton();
}

void SettingsExternalTools::loadSettings() {
  onBeginLoadSettings();

  onEndLoadSettings();
}

void SettingsExternalTools::saveSettings() {
  onBeginSaveSettings();

  onEndSaveSettings();
}
