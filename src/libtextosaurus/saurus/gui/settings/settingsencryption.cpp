// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "saurus/gui/settings/settingsencryption.h"

SettingsEncryption::SettingsEncryption(Settings* settings, QWidget* parent) : SettingsPanel(settings, parent) {
  m_ui.setupUi(this);
}

void SettingsEncryption::loadSettings() {}

void SettingsEncryption::saveSettings() {}
