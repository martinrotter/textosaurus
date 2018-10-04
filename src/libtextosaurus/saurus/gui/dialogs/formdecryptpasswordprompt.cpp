// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "saurus/gui/dialogs/formdecryptpasswordprompt.h"

#include "common/gui/guiutilities.h"
#include "common/miscellaneous/cryptofactory.h"
#include "common/miscellaneous/iconfactory.h"
#include "saurus/miscellaneous/application.h"

#include <QPushButton>

FormDecryptPasswordPrompt::FormDecryptPasswordPrompt(QFile& file, QWidget* parent) : QDialog(parent) {
  m_ui.setupUi(this);
  m_ui.m_buttonBox->button(QDialogButtonBox::StandardButton::Ok)->setEnabled(false);

  GuiUtilities::applyDialogProperties(*this, qApp->icons()->fromTheme(QSL("document-encrypted")));

  connect(m_ui.m_cbShowPassword, &QCheckBox::toggled, this, [this](bool checked) {
    m_ui.m_txtPassword->setEchoMode(checked ? QLineEdit::EchoMode::Normal : QLineEdit::EchoMode::Password);
  });
  connect(m_ui.m_txtPassword, &QLineEdit::textEdited, this, [this, &file](const QString& text) {
    bool pass_correct = CryptoFactory::isPasswordCorrect(text, file);
    m_ui.m_buttonBox->button(QDialogButtonBox::StandardButton::Ok)->setEnabled(pass_correct);
  });
}

FormDecryptPasswordPrompt::~FormDecryptPasswordPrompt() {}

QString FormDecryptPasswordPrompt::password() const {
  return m_ui.m_txtPassword->text();
}

QString FormDecryptPasswordPrompt::getPasswordFromUser(QFile& file, bool& ok) {
  FormDecryptPasswordPrompt prompt(file, qApp->mainFormWidget());

  ok = prompt.exec() == QDialog::DialogCode::Accepted;
  return prompt.password();
}
