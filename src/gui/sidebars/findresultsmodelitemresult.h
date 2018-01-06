// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef FINDRESULTSMODELITEMRESULT_H
#define FINDRESULTSMODELITEMRESULT_H

#include "gui/sidebars/findresultsmodelitem.h"

class TextEditor;

class FindResultsModelItemResult : public FindResultsModelItem {
  Q_OBJECT

  public:
    explicit FindResultsModelItemResult(QObject* parent = nullptr);

    virtual QVariant data(int role) const override;
    TextEditor* editor() const;
};

#endif // FINDRESULTSMODELITEMRESULT_H
