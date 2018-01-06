// For license of this file, see <project-root-folder>/LICENSE.md.

#include "gui/sidebars/findresultsmodelitemresult.h"

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
