// For license of this file, see <project-root-folder>/LICENSE.md.

#include "gui/statusbar.h"

#include "definitions/definitions.h"
#include "miscellaneous/textfactory.h"

StatusBar::StatusBar(QWidget* parent) : QStatusBar(parent), m_lblInfo(new QLabel(this)), m_infoEncoding(QString()),
  m_infoEol(QString()), m_infoLexer(QString()) {
  setSizeGripEnabled(false);

  m_lblInfo->setStyleSheet(QSL("QLabel { margin-right: 16px; }"));

  addPermanentWidget(m_lblInfo);
}

StatusBar::~StatusBar() {
  qDebug("Destroying StatusBar instance.");
}

void StatusBar::setEol(int eol_type) {
  m_infoEol = TextFactory::eolDescriptionFromCode(eol_type);
}

void StatusBar::setEncoding(const QString& encoding) {
  m_infoEncoding = encoding;

  updateInfo();
}

void StatusBar::setFileType(const QString& file_type) {
  m_infoLexer = file_type;

  updateInfo();
}

void StatusBar::updateInfo() {
  QStringList lst; lst << m_infoEol << m_infoLexer << m_infoEncoding;

  lst.removeAll(QString());
  m_lblInfo->setText(lst.join(QSL(" \u2022 ")));
}
