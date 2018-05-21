// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "saurus/gui/editortab.h"

#include "saurus/gui/tabwidget.h"
#include "saurus/miscellaneous/textapplication.h"

#include <QLayout>

EditorTab::EditorTab(TextApplication* text_app, TextEditor* editor)
  : Tab(text_app->tabWidget()), m_editor(editor) {
  QVBoxLayout* lay = new QVBoxLayout(this);

  lay->setMargin(0);
  lay->setSpacing(0);
  lay->addWidget(m_editor, 1);
}

EditorTab::EditorTab(TextApplication* text_app)
  : EditorTab(text_app, new TextEditor(text_app, this)) {}

TextEditor* EditorTab::primaryEditor() const {
  return m_editor;
}

QList<TextEditor*> EditorTab::allEditors() const {
  return QList<TextEditor*>() << primaryEditor();
}

int EditorTab::countOfEditors() const {
  return 1;
}

TabTyp EditorTab::tabType() const {
  return TabTyp::TextEditor;
}

void EditorTab::closeEvent(QCloseEvent* event) {
  bool ok = false;

  m_editor->closeEditor(&ok);

  if (!ok) {
    event->ignore();
  }
  else {
    Tab::closeEvent(event);
  }
}
