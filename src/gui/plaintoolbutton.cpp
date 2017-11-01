// For license of this file, see <project-root-folder>/LICENSE.md.

#include "gui/plaintoolbutton.h"

#include <QAction>
#include <QPainter>
#include <QPaintEvent>
#include <QRect>
#include <QStyle>
#include <QStyleOption>
#include <QToolButton>

PlainToolButton::PlainToolButton(QWidget* parent) : QToolButton(parent),
  m_paddingBottom(0), m_paddingLeft(0), m_paddingRight(0), m_paddingTop(0) {}

PlainToolButton::~PlainToolButton() {}

void PlainToolButton::paintEvent(QPaintEvent* e) {
  Q_UNUSED(e)
  QPainter p(this);
  QSize siz = size();
  QRect rect(m_paddingLeft, m_paddingTop, siz.width() - m_paddingLeft - m_paddingRight, siz.height() - m_paddingTop - m_paddingBottom);

  // Set padding.
  //rect.adjust(m_padding, m_padding, -m_padding, -m_padding);

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
