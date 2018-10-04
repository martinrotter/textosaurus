// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "saurus/gui/dialogs/formdecryptpasswordprompt.h"

#include "common/gui/guiutilities.h"
#include "common/miscellaneous/cryptofactory.h"
#include "common/miscellaneous/iconfactory.h"
#include "saurus/miscellaneous/application.h"

#include <QPushButton>

FormDecryptPasswordPrompt::FormDecryptPasswordPrompt(QFile& file, QWidget* parent) : QDialog(parent) {
  m_ui.setupUi(this);
  m_ui.m_buttonBox->button(QDialogButtonBox::StandardButton::Ok)->setText(tr("Decrypt && Open File"));
  m_ui.m_buttonBox->button(QDialogButtonBox::StandardButton::Ok)->setEnabled(false);
  m_ui.m_tbPassword->lineEdit()->setPlaceholderText(tr("Enter Decryption Password"));
  m_ui.m_tbPassword->lineEdit()->setEchoMode(QLineEdit::EchoMode::Password);
  m_ui.m_tbPassword->setStatus(WidgetWithStatus::StatusType::Error, tr("Entered password is incorrect."));
  m_ui.m_tbPassword->setLayoutDirection(Qt::LayoutDirection::RightToLeft);

  GuiUtilities::applyDialogProperties(*this, qApp->icons()->fromTheme(QSL("multipart-encrypted")));

  connect(m_ui.m_cbShowPassword, &QCheckBox::toggled, this, [this](bool checked) {
    m_ui.m_tbPassword->lineEdit()->setEchoMode(checked ? QLineEdit::EchoMode::Normal : QLineEdit::EchoMode::Password);
    m_ui.m_tbPassword->setFocus();
  });
  connect(m_ui.m_tbPassword->lineEdit(), &BaseLineEdit::submitted, this, [this]() {
    if (m_ui.m_buttonBox->button(QDialogButtonBox::StandardButton::Ok)->isEnabled()) {
      m_ui.m_buttonBox->button(QDialogButtonBox::StandardButton::Ok)->click();
    }
  });
  connect(m_ui.m_tbPassword->lineEdit(), &QLineEdit::textEdited, this, [this, &file](const QString& text) {
    bool pass_correct;

    try {
      pass_correct = CryptoFactory::isPasswordCorrect(text, file);
    }
    catch (...) {
      pass_correct = false;
    }

    m_ui.m_buttonBox->button(QDialogButtonBox::StandardButton::Ok)->setEnabled(pass_correct);

    if (pass_correct) {
      m_ui.m_tbPassword->setStatus(WidgetWithStatus::StatusType::Ok, tr("Nice! This is correct password."));
    }
    else {
      m_ui.m_tbPassword->setStatus(WidgetWithStatus::StatusType::Error, tr("Entered password is incorrect."));
    }
  });

  m_ui.m_tbPassword->setFocus();
}

QString FormDecryptPasswordPrompt::password() const {
  return m_ui.m_tbPassword->lineEdit()->text();
}

QString FormDecryptPasswordPrompt::getPasswordFromUser(QFile& file, bool& ok) {
  FormDecryptPasswordPrompt prompt(file, qApp->mainFormWidget());

  ok = prompt.exec() == QDialog::DialogCode::Accepted;
  return prompt.password();
}
