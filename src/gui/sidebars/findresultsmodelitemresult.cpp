// For license of this file, see <project-root-folder>/LICENSE.md.

#include "gui/sidebars/findresultsmodelitemresult.h"

#include "gui/sidebars/findresultsmodelitemeditor.h"

#include <QColor>
#include <QVariant>

FindResultsModelItemResult::FindResultsModelItemResult(QObject* parent) : FindResultsModelItem(parent) {}

QVariant FindResultsModelItemResult::data(int role) const {
  switch (role) {
    case Qt::ItemDataRole::DisplayRole:
      return "result";

    default:
      return QVariant();
  }
}

TextEditor* FindResultsModelItemResult::editor() const {
  FindResultsModelItemEditor* item_editor = qobject_cast<FindResultsModelItemEditor*>(parentItem());

  if (item_editor != nullptr) {
    return item_editor->editor();
  }
  else {
    return nullptr;
  }
}
