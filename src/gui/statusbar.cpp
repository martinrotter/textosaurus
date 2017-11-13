// For license of this file, see <project-root-folder>/LICENSE.md.

#include "gui/statusbar.h"

#include "definitions/definitions.h"

StatusBar::StatusBar(QWidget* parent) : QStatusBar(parent) {
  setSizeGripEnabled(false);

  m_lblEncoding = new QLabel(this);
  m_lblFileType = new QLabel(this);

  m_lblEncoding->setStyleSheet(QSL("QLabel { margin-right: 16px; }"));
  m_lblFileType->setStyleSheet(QSL("QLabel { margin-right: 16px; }"));

  m_lblFileType->setToolTip(tr("Active syntax highlighter"));
  m_lblEncoding->setToolTip(tr("Output encoding"));

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
