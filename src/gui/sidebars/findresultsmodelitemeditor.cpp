// For license of this file, see <project-root-folder>/LICENSE.md.

#include "findresultsmodelitemeditor.h"

#include <QColor>
#include <QVariant>

FindResultsModelItemEditor::FindResultsModelItemEditor(TextEditor* editor, QObject* parent)
  : FindResultsModelItem(parent), m_editor(editor) {}

QVariant FindResultsModelItemEditor::data(int role) const {
  switch (role) {
    case Qt::ItemDataRole::DisplayRole:
      return "editor";

    case Qt::ItemDataRole::BackgroundRole:
      return QColor(0, 200, 0, 10);

    default:
      return QVariant();
  }
}

TextEditor* FindResultsModelItemEditor::editor() const {
  return m_editor.data();
}
