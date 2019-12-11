// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "saurus/gui/dialogs/formfindreplace.h"

#include "common/network-web/webfactory.h"
#include "saurus/gui/sidebars/findresultssidebar.h"
#include "saurus/gui/tabwidget.h"
#include "saurus/gui/texteditor.h"
#include "saurus/miscellaneous/application.h"
#include "saurus/miscellaneous/textapplication.h"

#include <QClipboard>

FormFindReplace::FormFindReplace(TextApplication* app, QWidget* parent) : QDialog(parent), m_application(app) {
  m_ui.setupUi(this);
  m_ui.m_lblResult->setStyleSheet(QSL("color: red;"));

  setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowStaysOnTopHint | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);

  connect(m_ui.m_btnCount, &QPushButton::clicked, this, &FormFindReplace::displayCount);
  connect(m_ui.m_btnFindNext, &QPushButton::clicked, this, &FormFindReplace::searchNext);
  connect(m_ui.m_btnFindPrevious, &QPushButton::clicked, this, &FormFindReplace::searchPrevious);
  connect(m_ui.m_txtSearchPhrase, &BaseLineEdit::submitted, this, &FormFindReplace::searchNext);
  connect(m_ui.m_lblRegexInfo, &QLabel::linkActivated, qApp->web(), &WebFactory::openUrlInExternalBrowser);
  connect(m_ui.m_btnFindAll, &QPushButton::clicked, this, &FormFindReplace::searchAll);
  connect(m_ui.m_btnReplaceAll, &QPushButton::clicked, this, &FormFindReplace::replaceAll);
  connect(m_ui.m_txtSearchPhrase, &BaseLineEdit::textChanged, this, &FormFindReplace::clearTargetRange);
  connect(m_ui.m_btnReplaceNext, &QPushButton::clicked, this, &FormFindReplace::searchReplaceNext);
}

void FormFindReplace::display() {
  show();
  activateWindow();
  raise();

  TextEditor* editor = m_application->tabWidget()->currentEditor();

  if (editor != nullptr && !editor->selectionEmpty()) {
    m_ui.m_txtSearchPhrase->setText(editor->getSelText());
  }
  else if (!qApp->clipboard()->text().isEmpty()) {
    m_ui.m_txtSearchPhrase->setText(qApp->clipboard()->text());
  }

  m_ui.m_lblResult->clear();
  m_ui.m_txtSearchPhrase->setFocus();
  m_ui.m_txtSearchPhrase->selectAll();
  m_ui.m_txtReplaceString->clear();

  clearTargetRange();
}

void FormFindReplace::clearTargetRange() {
  TextEditor* editor = m_application->tabWidget()->currentEditor();

  if (editor != nullptr) {
    editor->setTargetRange(-1, -1);
  }
}

void FormFindReplace::displayCount() {
  TextEditor* editor = m_application->tabWidget()->currentEditor();

  if (editor == nullptr || m_ui.m_txtSearchPhrase->text().isEmpty()) {
    m_ui.m_lblResult->setText("Either no input or no text editor active.");
    return;
  }

  int search_flags = extractFlags();

  QPair<int, int> found_range;
  int start_position = 0;
  int end_position = int(editor->length());
  int count = 0;

  forever {
    found_range = editor->findText(search_flags,
                                   m_ui.m_txtSearchPhrase->text().toUtf8().constData(),
                                   start_position,
                                   end_position);

    if (found_range.first >= 0) {
      start_position = found_range.first == found_range.second ? found_range.second + 1 : found_range.second;
      count++;
    }
    else {
      break;
    }
  }

  m_ui.m_lblResult->setText(tr("Count: %1 matches.").arg(count));
}

void FormFindReplace::searchNext() {
  // NOTE: informace scite
  // https://github.com/LuaDist/scite/blob/fab4a6321c52c6ea8d1e1eab9c4fee86f7388697/src/SciTEBase.cxx#L1052

  searchOne(false);
}

void FormFindReplace::searchPrevious() {
  searchOne(true);
}

void FormFindReplace::replaceAll() {
  TextEditor* editor = m_application->tabWidget()->currentEditor();

  if (editor == nullptr || m_ui.m_txtSearchPhrase->text().isEmpty()) {
    m_ui.m_lblResult->setText("Either no input or no text editor active.");
    return;
  }

  int start_position = 0, replacements_made = 0, search_flags = extractFlags();

  while (start_position < editor->length()) {
    QPair<int, int> found_range = editor->findText(search_flags,
                                                   m_ui.m_txtSearchPhrase->text().toUtf8().constData(),
                                                   start_position,
                                                   int(editor->length()));

    if (found_range.first >= 0) {
      sptr_t replacement_length;

      editor->setTargetRange(found_range.first, found_range.second);

      // We replace.
      if (m_ui.m_rbModeRegex->isChecked()) {
        replacement_length = editor->replaceTargetRE(-1, m_ui.m_txtReplaceString->text().toUtf8().constData());
      }
      else {
        replacement_length = editor->replaceTarget(-1, m_ui.m_txtReplaceString->text().toUtf8().constData());
      }

      replacement_length += found_range.first;
      start_position = found_range.first == found_range.second ? (int(replacement_length) + 1) : int(replacement_length);
      replacements_made++;
    }
    else {
      break;
    }
  }

  if (replacements_made == 0) {
    m_ui.m_lblResult->setText("Nothing replaced.");
  }
  else {
    m_ui.m_lblResult->setText(tr("Replaced %n occurrence(s).", "", replacements_made));
  }
}

void FormFindReplace::searchReplaceNext() {
  searchReplaceOne(false);
}

void FormFindReplace::searchAll() {
  TextEditor* editor = m_application->tabWidget()->currentEditor();

  if (editor == nullptr || m_ui.m_txtSearchPhrase->text().isEmpty()) {
    m_ui.m_lblResult->setText("Either no input or no text editor active.");
    return;
  }

  QList<QPair<int, int>> found_ranges;
  int start_position = 0, end_position = int(editor->length()), search_flags = extractFlags();

  while (true) {
    QPair<int, int> found_range = editor->findText(search_flags,
                                                   m_ui.m_txtSearchPhrase->text().toUtf8().constData(),
                                                   start_position,
                                                   end_position);

    if (found_range.first >= 0) {
      found_ranges.append(found_range);
      start_position = found_range.first == found_range.second ? (found_range.second + 1) : found_range.second;
    }
    else {
      break;
    }
  }

  if (found_ranges.isEmpty()) {
    m_ui.m_lblResult->setText(tr("Nothing found."));
  }
  else {
    m_application->findResultsSidebar()->addResults(editor, found_ranges);
    close();
  }
}

int FormFindReplace::extractFlags() {
  int search_flags = 0;

  search_flags |= m_ui.m_checkMatchWholeWords->isChecked() ? SCFIND_WHOLEWORD : 0;
  search_flags |= m_ui.m_rbModeRegex->isChecked() ? SCFIND_CXX11REGEX | SCFIND_REGEXP : 0;
  search_flags |= m_ui.m_checkCaseSensitiveSearch->isChecked() ? SCFIND_MATCHCASE : 0;

  return search_flags;
}

void FormFindReplace::searchReplaceOne(bool reverse) {
  TextEditor* editor = m_application->tabWidget()->currentEditor();

  if (editor == nullptr || m_ui.m_txtSearchPhrase->text().isEmpty()) {
    m_ui.m_lblResult->setText("Either no input or no text editor active.");
    return;
  }

  int start_position, end_position;

  if (reverse) {
    start_position = int(editor->selectionStart());
    end_position = 0;
  }
  else {
    start_position = int(editor->selectionEnd());
    end_position = int(editor->length());
  }

  int search_flags = extractFlags();

  QPair<int, int> found_range = editor->findText(search_flags,
                                                 m_ui.m_txtSearchPhrase->text().toUtf8().constData(),
                                                 start_position,
                                                 end_position);

  if (found_range.first >= 0) {
    m_ui.m_lblResult->clear();

    int replacement_length;

    editor->setTargetRange(found_range.first, found_range.second);

    // We replace.
    if (m_ui.m_rbModeRegex->isChecked()) {
      replacement_length = int(editor->replaceTargetRE(-1, m_ui.m_txtReplaceString->text().toUtf8().constData()));
    }
    else {
      replacement_length = int(editor->replaceTarget(-1, m_ui.m_txtReplaceString->text().toUtf8().constData()));
    }

    // We highlight and scroll.
    editor->ensureVisible(editor->lineFromPosition(found_range.first));
    editor->setSel(found_range.first, found_range.first + replacement_length);
    return;
  }

  if (m_ui.m_checkWrapSearch->isChecked()) {
    // Second pass, wrapping around.
    if (reverse) {
      // From end to current position.
      start_position = int(editor->length());
      end_position = static_cast<int>(editor->selectionEnd());
    }
    else {
      // From beginning to current position.
      start_position = 0;
      end_position = static_cast<int>(editor->selectionStart());
    }

    found_range = editor->findText(search_flags,
                                   m_ui.m_txtSearchPhrase->text().toUtf8().constData(),
                                   start_position,
                                   end_position);

    if (found_range.first >= 0) {
      m_ui.m_lblResult->clear();

      int replacement_length;

      editor->setTargetRange(found_range.first, found_range.second);

      // We replace.
      if (m_ui.m_rbModeRegex->isChecked()) {
        replacement_length = int(editor->replaceTargetRE(-1, m_ui.m_txtReplaceString->text().toUtf8().constData()));
      }
      else {
        replacement_length = int(editor->replaceTarget(-1, m_ui.m_txtReplaceString->text().toUtf8().constData()));
      }

      // We highlight and scroll.
      editor->ensureVisible(editor->lineFromPosition(found_range.first));
      editor->setSel(found_range.first, found_range.first + replacement_length);
      return;
    }
  }

  // Next occurrence was not found.
  m_ui.m_lblResult->setText(tr("Cannot find the text \"%1\".").arg(m_ui.m_txtSearchPhrase->text()));
}

void FormFindReplace::searchOne(bool reverse) {
  TextEditor* editor = m_application->tabWidget()->currentEditor();

  if (editor == nullptr || m_ui.m_txtSearchPhrase->text().isEmpty()) {
    m_ui.m_lblResult->setText("Either no input or no text editor active.");
    return;
  }

  int start_position, end_position;

  if (reverse) {
    start_position = int(editor->selectionStart());
    end_position = 0;
  }
  else {
    start_position = static_cast<int>(editor->selectionEnd());
    end_position = int(editor->length());
  }

  int search_flags = extractFlags();

  QPair<int, int> found_range = editor->findText(search_flags,
                                                 m_ui.m_txtSearchPhrase->text().toUtf8().constData(),
                                                 start_position,
                                                 end_position);

  if (found_range.first >= 0) {
    m_ui.m_lblResult->clear();
    editor->ensureVisible(editor->lineFromPosition(found_range.first));
    editor->setSel(found_range.first, found_range.second);
    return;
  }

  if (m_ui.m_checkWrapSearch->isChecked()) {
    // Second pass, wrapping around.
    if (reverse) {
      // From end to current position.
      start_position = int(editor->length());
      end_position = static_cast<int>(editor->selectionEnd());
    }
    else {
      // From beginning to current position.
      start_position = 0;
      end_position = static_cast<int>(editor->selectionStart());
    }

    found_range = editor->findText(search_flags,
                                   m_ui.m_txtSearchPhrase->text().toUtf8().constData(),
                                   start_position,
                                   end_position);

    if (found_range.first >= 0) {
      m_ui.m_lblResult->clear();
      editor->ensureVisible(editor->lineFromPosition(found_range.first));
      editor->setSel(found_range.first, found_range.second);
      return;
    }
  }

  // Next occurrence was not found.
  m_ui.m_lblResult->setText(tr("Cannot find the text \"%1\".").arg(m_ui.m_txtSearchPhrase->text()));
}
