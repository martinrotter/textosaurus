// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifndef LINEEDITWITHSTATUS_H
#define LINEEDITWITHSTATUS_H

#include "common/gui/widgetwithstatus.h"

#include "common/gui/baselineedit.h"

class LineEditWithStatus : public WidgetWithStatus {
  Q_OBJECT

  public:
    explicit LineEditWithStatus(QWidget* parent = nullptr);
    virtual ~LineEditWithStatus() = default;

    // Access to line edit.
    inline BaseLineEdit* lineEdit() const {
      return static_cast<BaseLineEdit*>(m_wdgInput);
    }

};

#endif // LINEEDITWITHSTATUS_H
