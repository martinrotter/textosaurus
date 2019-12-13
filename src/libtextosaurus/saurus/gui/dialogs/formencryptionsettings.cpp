// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "saurus/gui/dialogs/formencryptionsettings.h"

#include "common/gui/guiutilities.h"
#include "common/miscellaneous/iconfactory.h"
#include "saurus/miscellaneous/application.h"

#include <QPushButton>

FormEncryptionSettings::FormEncryptionSettings(const QString& password, QWidget* parent) : QDialog(parent) {
  m_ui.setupUi(this);
  m_ui.m_buttonBox->button(QDialogButtonBox::StandardButton::Ok)->setEnabled(false);
  m_ui.m_tbPassword->lineEdit()->setPlaceholderText(tr("Enter Encryption Password"));
  m_ui.m_tbPassword->lineEdit()->setEchoMode(QLineEdit::EchoMode::Password);
  m_ui.m_tbPassword->setStatus(WidgetWithStatus::StatusType::Error, tr("Entered password is incorrect."));
  m_ui.m_tbPassword->setLayoutDirection(Qt::LayoutDirection::RightToLeft);

  GuiUtilities::applyDialogProperties(*this, qApp->icons()->fromTheme(QSL("multipart-encrypted")));
  GuiUtilities::disableCloseButton(*this);

  connect(m_ui.m_cbShowPassword, &QCheckBox::toggled, this, [this](bool checked) {
    m_ui.m_tbPassword->lineEdit()->setEchoMode(checked ? QLineEdit::EchoMode::Normal : QLineEdit::EchoMode::Password);
    m_ui.m_tbPassword->setFocus();
  });
  connect(m_ui.m_tbPassword->lineEdit(), &BaseLineEdit::submitted, this, [this]() {
    if (m_ui.m_buttonBox->button(QDialogButtonBox::StandardButton::Ok)->isEnabled()) {
      m_ui.m_buttonBox->button(QDialogButtonBox::StandardButton::Ok)->click();
    }
  });

  connect(m_ui.m_gbEncryption, &QGroupBox::toggled, this, &FormEncryptionSettings::updateResults);
  connect(m_ui.m_tbPassword->lineEdit(), &QLineEdit::textChanged, this, &FormEncryptionSettings::updateResults);

  m_ui.m_gbEncryption->setChecked(!password.isEmpty());
  m_ui.m_tbPassword->lineEdit()->setText(password);
}

QString FormEncryptionSettings::encryptionPassword() const {
  return m_ui.m_gbEncryption->isChecked() ? m_ui.m_tbPassword->lineEdit()->text() : QString();
}

void FormEncryptionSettings::updateResults() {
  bool encryption_enabled = m_ui.m_gbEncryption->isChecked();
  int passwd_size = m_ui.m_tbPassword->lineEdit()->text().size();

  m_ui.m_buttonBox->button(QDialogButtonBox::StandardButton::Ok)->setEnabled(!encryption_enabled ||
                                                                             passwd_size > 0);

  if (encryption_enabled) {
    if (passwd_size >= CRYPTO_RECOMMENDED_PASS_LENGTH) {
      m_ui.m_tbPassword->setStatus(WidgetWithStatus::StatusType::Ok, tr("Nice! You are good to go."));
    }
    else if (passwd_size > 0) {
      m_ui.m_tbPassword->setStatus(WidgetWithStatus::StatusType::Warning, tr("Your password is too short, but it's your call, boss!"));
    }
    else {
      m_ui.m_tbPassword->setStatus(WidgetWithStatus::StatusType::Error, tr("Enter some password."));
    }
  }
}
