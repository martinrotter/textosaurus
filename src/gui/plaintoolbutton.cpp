// For license of this file, see <project-root-folder>/LICENSE.md.

#include "gui/plaintoolbutton.h"

#include <QAction>
#include <QPainter>
#include <QPaintEvent>
#include <QStyle>
#include <QStyleOption>
#include <QToolButton>

PlainToolButton::PlainToolButton(QWidget* parent) : QToolButton(parent) {}

void PlainToolButton::paintEvent(QPaintEvent* e) {
  Q_UNUSED(e)
  QPainter p(this);
  QRect rect(m_paddingLeft, m_paddingTop,
             size().width() - m_paddingLeft - m_paddingRight, size().height() - m_paddingTop - m_paddingBottom);

  if (isEnabled()) {
    if (underMouse() || isChecked()) {
      p.setOpacity(0.7);
    }
  }
  else {
    p.setOpacity(0.3);
  }

  icon().paint(&p, rect);
}

void PlainToolButton::setPadding(int left, int top, int right, int bottom) {
  m_paddingLeft = left;
  m_paddingTop = top;
  m_paddingRight = right;
  m_paddingBottom = bottom;

  repaint();
}

void PlainToolButton::setChecked(bool checked) {
  QToolButton::setChecked(checked);
  repaint();
}

void PlainToolButton::reactOnActionChange(QAction* action) {
  if (action != nullptr) {
    setEnabled(action->isEnabled());
    setCheckable(action->isCheckable());
    setChecked(action->isChecked());
    setIcon(action->icon());
    setToolTip(action->toolTip());
  }
}

void PlainToolButton::reactOnSenderActionChange() {
  reactOnActionChange(qobject_cast<QAction*>(sender()));
}
