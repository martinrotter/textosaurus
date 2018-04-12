// For license of this file, see <project-root-folder>/LICENSE.md.

#include "saurus/gui/editortab.h"

#include "saurus/miscellaneous/textapplication.h"

#include <QLayout>

EditorTab::EditorTab(TextApplication* text_app, QWidget* parent)
  : Tab(text_app->tabWidget(), parent), m_editor(new TextEditor(text_app, this)) {
  QVBoxLayout* lay = new QVBoxLayout(this);

  lay->setMargin(0);
  lay->setSpacing(0);
  lay->addWidget(m_editor, 1);
}

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
