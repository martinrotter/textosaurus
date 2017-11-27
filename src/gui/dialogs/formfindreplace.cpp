// For license of this file, see <project-root-folder>/LICENSE.md.

#include "gui/dialogs/formfindreplace.h"

#include "gui/texteditor.h"
#include "miscellaneous/textapplication.h"

FormFindReplace::FormFindReplace(TextApplication* app, QWidget* parent) : QDialog(parent), m_application(app) {
  m_ui.setupUi(this);
  m_ui.m_lblResult->setStyleSheet(QSL("color: red; position: relative; top: 4px;"));

  setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowStaysOnTopHint | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);

  connect(m_ui.m_btnCount, &QPushButton::clicked, this, &FormFindReplace::displayCount);
  connect(m_ui.m_btnFindNext, &QPushButton::clicked, this, &FormFindReplace::searchNext);
  connect(m_ui.m_txtSearchPhrase, &BaseLineEdit::submitted, this, &FormFindReplace::searchNext);
}

FormFindReplace::~FormFindReplace() {}

void FormFindReplace::display() {
  show();
  activateWindow();
  raise();

  m_ui.m_txtSearchPhrase->setFocus();
}

void FormFindReplace::displayCount() {
  TextEditor* editor = m_application->currentEditor();

  if (editor == nullptr || m_ui.m_txtSearchPhrase->text().isEmpty()) {
    m_ui.m_lblResult->setText("Either no input or no text editor active.");
    return;
  }

  int search_flags = extractFlags();

  QPair<int, int> found_range;
  int start_position = 0;
  int end_position = editor->length();
  int count = 0;

  do {
    start_position = found_range.second;

    found_range = editor->findText(search_flags,
                                   m_ui.m_txtSearchPhrase->text().toUtf8().constData(),
                                   start_position,
                                   end_position);
  } while (found_range.first >= 0);

  m_ui.m_lblResult->setText(tr("Count: %1 matches.").arg(count));
}

void FormFindReplace::searchNext() {
  // NOTE: informace scite
  // https://github.com/LuaDist/scite/blob/fab4a6321c52c6ea8d1e1eab9c4fee86f7388697/src/SciTEBase.cxx#L1052

  search(m_ui.m_checkReverse->isChecked());
}

int FormFindReplace::extractFlags() {
  int search_flags = 0;

  search_flags |= m_ui.m_checkMatchWholeWords->isChecked() ? SCFIND_WHOLEWORD : 0;
  search_flags |= m_ui.m_rbModeRegex->isChecked() ? SCFIND_CXX11REGEX | SCFIND_REGEXP : 0;
  search_flags |= m_ui.m_checkCaseSensitiveSearch->isChecked() ? SCFIND_MATCHCASE : 0;

  return search_flags;
}

void FormFindReplace::search(bool reverse) {
  TextEditor* editor = m_application->currentEditor();

  if (editor == nullptr || m_ui.m_txtSearchPhrase->text().isEmpty()) {
    m_ui.m_lblResult->setText("Either no input or no text editor active.");
    return;
  }

  int start_position, end_position;

  if (reverse) {
    start_position = editor->selectionStart();
    end_position = 0;
  }
  else {
    start_position = static_cast<int>(editor->selectionEnd());
    end_position = editor->length();
  }

  int search_flags = extractFlags();

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
