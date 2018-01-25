// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef FINDRESULTSMODELITEMRESULT_H
#define FINDRESULTSMODELITEMRESULT_H

#include "gui/sidebars/findresultsmodelitem.h"

class TextEditor;

class FindResultsModelItemResult : public FindResultsModelItem {
  Q_OBJECT

  public:
    explicit FindResultsModelItemResult(const QString& found_text, int line, const QPair<int, int> range, QObject* parent = nullptr);

    virtual QVariant data(int role) const override;
    TextEditor* editor() const;

    QPair<int, int> range() const;

  private:
    QString m_resultText;
    int m_line;

    QPair<int, int> m_range;
};

#endif // FINDRESULTSMODELITEMRESULT_H
