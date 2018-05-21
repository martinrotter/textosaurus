// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "common/gui/plaintoolbutton.h"

#include "definitions/definitions.h"

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

  if (icon().isNull()) {
    if (m_explicitColor.isValid()) {
      p.fillRect(rect, m_explicitColor);
    }
    else {
      rect.adjust(0, 0, -1, -1);
      p.drawRect(rect);
      p.drawText(rect, QSL("T"), QTextOption(Qt::AlignCenter));
    }
  }
  else {
    icon().paint(&p, rect);
  }
}

QColor PlainToolButton::explicitColor() const {
  return m_explicitColor;
}

void PlainToolButton::setExplicitColor(const QColor& explicit_color) {
  m_explicitColor = explicit_color;

  repaint();
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
