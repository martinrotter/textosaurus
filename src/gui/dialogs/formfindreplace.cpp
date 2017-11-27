// For license of this file, see <project-root-folder>/LICENSE.md.

#include "gui/dialogs/formfindreplace.h"

#include "gui/texteditor.h"
#include "miscellaneous/textapplication.h"

FormFindReplace::FormFindReplace(TextApplication* app, QWidget* parent) : QDialog(parent), m_application(app) {
  m_ui.setupUi(this);

  m_ui.m_lblResult->setStyleSheet(QSL("color: red;"));

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

  search(m_ui.m_checkReverse->isChecked());
}

void FormFindReplace::search(bool reverse) {
  TextEditor* editor = m_application->currentEditor();

  if (editor == nullptr || editor->length() <= 0 || m_ui.m_txtSearchPhrase->text().isEmpty()) {
    m_ui.m_lblResult->clear();
    return;
  }

  int sel_start = editor->selectionStart();
  int sel_end = editor->selectionEnd();
  int start_position, end_position;

  if (reverse) {
    start_position = sel_start;
    end_position = 0;
  }
  else {
    start_position = static_cast<int>(sel_end);
    end_position = editor->length();
  }

  int search_flags = 0;

  search_flags |= m_ui.m_checkMatchWholeWords->isChecked() ? SCFIND_WHOLEWORD : 0;
  search_flags |= m_ui.m_rbModeRegex->isChecked() ? SCFIND_CXX11REGEX | SCFIND_REGEXP : 0;
  search_flags |= m_ui.m_checkCaseSensitiveSearch->isChecked() ? SCFIND_MATCHCASE : 0;

  QPair<int, int> found_range = editor->findText(search_flags,
                                                 m_ui.m_txtSearchPhrase->text().toUtf8().constData(),
                                                 start_position,
                                                 end_position);

  if (found_range.first < 0) {
    // Next occurrence was not found.
    m_ui.m_lblResult->setText(tr("Cannot find the text \"%1\".").arg(m_ui.m_txtSearchPhrase->text()));
  }
  else {
    m_ui.m_lblResult->clear();
    editor->setSelection(found_range.first, found_range.second);
    editor->scrollCaret();
  }

  /*
     editor->setTargetRange(found_range.first, found_range.second);
     editor->setSelection(found_range.first, found_range.second);

     auto aa = editor->replaceTargetRE(-1, m_ui.m_txtReplaceString->text().toUtf8().constData());

     editor->setSelection(found_range.first, editor->targetEnd());*/
}
