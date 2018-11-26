// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "common/miscellaneous/cryptofactory.h"

#include "common/gui/guiutilities.h"
#include "common/miscellaneous/settings.h"
#include "definitions/definitions.h"
#include "saurus/gui/settings/settingsencryption.h"

SettingsEncryption::SettingsEncryption(Settings* settings, QWidget* parent) : SettingsPanel(settings, parent) {
  m_ui.setupUi(this);

  m_ui.m_lblInfo->setText(tr("%1 uses \"openssl\" utility to encrypt and decrypt your documents. "
                             "Your password is passed to the utility via \"standard input\" and the document "
                             "which should be encrypted (or decrypted) is passed via temporary file which is always "
                             "deleted after the operation is done.").arg(APP_NAME));

  GuiUtilities::setLabelAsNotice(*m_ui.m_lblInfo, false);
}

void SettingsEncryption::loadSettings() {
  // Load values.
  m_ui.m_txtStatus->setText(CryptoFactory::openSslVersion());
  m_ui.m_cmbCipher->addItems(CryptoFactory::ciphers());

  m_ui.m_cmbCipher->setCurrentText(settings()->value(GROUP(General), SETTING(General::EncryptionCipher)).toString());
  m_ui.m_cbPbkdf2->setChecked(settings()->value(GROUP(General), SETTING(General::EncryptionUsePbkdf2)).toBool());

  connect(m_ui.m_cbPbkdf2, &QCheckBox::toggled, this, &SettingsEncryption::dirtifySettings);
  connect(m_ui.m_cmbCipher, &QComboBox::currentTextChanged, this, &SettingsEncryption::dirtifySettings);
}

void SettingsEncryption::saveSettings() {
  // Save values.
  settings()->setValue(GROUP(General), General::EncryptionCipher, m_ui.m_cmbCipher->currentText());
  settings()->setValue(GROUP(General), General::EncryptionUsePbkdf2, m_ui.m_cbPbkdf2->isChecked());
}
