// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "common/gui/lineeditwithstatus.h"

#include "common/gui/baselineedit.h"
#include "common/gui/plaintoolbutton.h"

#include <QHBoxLayout>

LineEditWithStatus::LineEditWithStatus(QWidget* parent) : WidgetWithStatus(parent) {
  m_wdgInput = new BaseLineEdit(this);
  setFocusProxy(m_wdgInput);

  // Set correct size for the tool button.
  const int txt_input_height = m_wdgInput->sizeHint().height();

  m_btnStatus->setFixedSize(txt_input_height, txt_input_height);

  // Compose the layout.
  m_layout->addWidget(m_wdgInput);
  m_layout->addWidget(m_btnStatus);
}
