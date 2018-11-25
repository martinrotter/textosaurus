// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "common/miscellaneous/cryptofactory.h"
#include "saurus/gui/settings/settingsencryption.h"

SettingsEncryption::SettingsEncryption(Settings* settings, QWidget* parent) : SettingsPanel(settings, parent) {
  m_ui.setupUi(this);
}

void SettingsEncryption::loadSettings() {
  // Load values.
  m_ui.m_txtStatus->setText(CryptoFactory::openSslVersion());

  //connect(m_ui.m_txtDecryption, &QPlainTextEdit::textChanged, this, &SettingsEncryption::dirtifySettings);
  //connect(m_ui.m_txtEncryption, &QPlainTextEdit::textChanged, this, &SettingsEncryption::dirtifySettings);
}

void SettingsEncryption::saveSettings() {
  // Save values.
}
