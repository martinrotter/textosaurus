// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "saurus/gui/dialogs/formdecryptpasswordprompt.h"

#include "common/gui/guiutilities.h"
#include "common/miscellaneous/iconfactory.h"
#include "saurus/miscellaneous/application.h"

#include <QPushButton>

FormDecryptPasswordPrompt::FormDecryptPasswordPrompt(QFile& file, QWidget* parent) : QDialog(parent) {
  m_ui.setupUi(this);
  m_ui.m_buttonBox->button(QDialogButtonBox::StandardButton::Ok)->setEnabled(false);

  GuiUtilities::applyDialogProperties(*this, qApp->icons()->fromTheme(QSL("document-encrypted")));

  connect(m_ui.m_txtPassword, &QLineEdit::textChanged, [this](const QString& text) {
    m_ui.m_buttonBox->button(QDialogButtonBox::StandardButton::Ok)->setEnabled(!text.isEmpty());
  });
}

FormDecryptPasswordPrompt::~FormDecryptPasswordPrompt() {}

QString FormDecryptPasswordPrompt::getPasswordFromUser(QFile& file, bool* ok) {
  FormDecryptPasswordPrompt prompt(file, qApp->mainFormWidget());

  prompt.show();
}
