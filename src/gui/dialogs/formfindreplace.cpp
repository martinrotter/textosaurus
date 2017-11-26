// For license of this file, see <project-root-folder>/LICENSE.md.

#include "gui/dialogs/formfindreplace.h"

#include "gui/texteditor.h"
#include "miscellaneous/textapplication.h"

FormFindReplace::FormFindReplace(TextApplication* app, QWidget* parent) : QDialog(parent), m_application(app) {
  m_ui.setupUi(this);

  setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowStaysOnTopHint | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
  setWindowOpacity(0.8);

  connect(m_ui.m_btnFindNext, &QPushButton::clicked, this, &FormFindReplace::searchNext);
  connect(m_ui.m_txtSearchPhrase, &BaseLineEdit::submitted, this, &FormFindReplace::searchNext);
}

FormFindReplace::~FormFindReplace() {}

void FormFindReplace::display() {
  show();
  activateWindow();
  raise();
}

void FormFindReplace::searchNext() {
  // NOTE: informace scite
  // https://github.com/LuaDist/scite/blob/fab4a6321c52c6ea8d1e1eab9c4fee86f7388697/src/SciTEBase.cxx#L1052

  TextEditor* editor = m_application->currentEditor();

  if (editor == nullptr || editor->length() == 0 || m_ui.m_txtSearchPhrase->text().isEmpty()) {
    return;
  }
}
