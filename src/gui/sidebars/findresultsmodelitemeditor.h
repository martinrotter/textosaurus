// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef FINDRESULTSMODELITEMEDITOR_H
#define FINDRESULTSMODELITEMEDITOR_H

#include "gui/sidebars/findresultsmodelitem.h"

class FindResultsModelItemEditor : public FindResultsModelItem {
  Q_OBJECT

  public:
    explicit FindResultsModelItemEditor(QObject* parent = nullptr);

    virtual QVariant data(int role) const override;
};

#endif // FINDRESULTSMODELITEMEDITOR_H
