// For license of this file, see <project-root-folder>/LICENSE.md.

#include "gui/sidebars/findresultsmodelitemresult.h"

#include "gui/sidebars/findresultsmodelitemeditor.h"

#include <QColor>
#include <QVariant>

FindResultsModelItemResult::FindResultsModelItemResult(const QString& found_text, int line, const QPair<int, int> range, QObject* parent)
  : FindResultsModelItem(parent), m_resultText(found_text), m_line(line), m_range(range) {}

QVariant FindResultsModelItemResult::data(int role) const {
  switch (role) {
    case Qt::ItemDataRole::DisplayRole:
      return QString("Line: %4: \"%1\" (%2 •• %3)").arg(m_resultText,
                                                        QString::number(m_range.first),
                                                        QString::number(m_range.second),
                                                        QString::number(m_line));

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

QPair<int, int> FindResultsModelItemResult::range() const {
  return m_range;
}
