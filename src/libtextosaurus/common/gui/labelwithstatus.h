// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifndef LABELWITHSTATUS_H
#define LABELWITHSTATUS_H

#include "common/gui/widgetwithstatus.h"

#include <QLabel>

class LabelWithStatus : public WidgetWithStatus {
  Q_OBJECT

  public:

    // Constructors and destructors.
    explicit LabelWithStatus(QWidget* parent = nullptr);
    virtual ~LabelWithStatus() = default;

    void setStatus(StatusType status, const QString& label_text, const QString& status_text);

    // Access to label.
    QLabel* label() const;
};

inline QLabel* LabelWithStatus::label() const {
  return qobject_cast<QLabel*>(m_wdgInput);
}

#endif // LABELWITHSTATUS_H
