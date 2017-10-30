// For license of this file, see <project-root-folder>/LICENSE.md.

#include "gui/statusbar.h"

StatusBar::StatusBar(QWidget* parent) : QStatusBar(parent) {
  setSizeGripEnabled(false);
  setContentsMargins(0, 0, 5, 0);

  m_lblEncoding = new QLabel(this);
  m_lblFileType = new QLabel(this);

  addPermanentWidget(m_lblFileType);
  addPermanentWidget(m_lblEncoding);
}

StatusBar::~StatusBar() {
  qDebug("Destroying StatusBar instance.");
}

void StatusBar::setEncoding(const QString& encoding) {
  m_lblEncoding->setText(encoding);
  m_lblEncoding->setVisible(!encoding.isEmpty());
}

void StatusBar::setFileType(const QString& file_type) {
  m_lblFileType->setText(file_type);
  m_lblFileType->setVisible(!file_type.isEmpty());
}

bool StatusBar::eventFilter(QObject* watched, QEvent* event) {
  Q_UNUSED(watched)
  Q_UNUSED(event)

  return false;
}
