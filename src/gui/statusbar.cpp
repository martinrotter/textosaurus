// For license of this file, see <project-root-folder>/LICENSE.md.

#include "gui/statusbar.h"

#include "gui/dialogs/formmain.h"
#include "gui/plaintoolbutton.h"
#include "gui/tabwidget.h"
#include "miscellaneous/iconfactory.h"

#include <QLabel>
#include <QProgressBar>
#include <QToolButton>

StatusBar::StatusBar(QWidget* parent) : QStatusBar(parent) {
  setSizeGripEnabled(false);
  setContentsMargins(2, 0, 2, 2);

  m_barProgressDownload = new QProgressBar(this);
  m_barProgressDownload->setTextVisible(true);
  m_barProgressDownload->setFixedWidth(100);
  m_barProgressDownload->setVisible(false);
  m_barProgressDownload->setObjectName(QSL("m_barProgressDownload"));
  m_lblProgressDownload = new QLabel(this);
  m_lblProgressDownload->setText("Downloading files in background");
  m_lblProgressDownload->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
  m_lblProgressDownload->setVisible(false);
  m_lblProgressDownload->setObjectName(QSL("m_lblProgressDownload"));
  m_lblProgressDownload->installEventFilter(this);
  m_barProgressDownload->installEventFilter(this);

  addPermanentWidget(m_lblProgressDownload);
  addPermanentWidget(m_barProgressDownload);
}

StatusBar::~StatusBar() {
  qDebug("Destroying StatusBar instance.");
}

bool StatusBar::eventFilter(QObject* watched, QEvent* event) {
  if (watched == m_lblProgressDownload || watched == m_barProgressDownload) {
    if (event->type() == QEvent::MouseButtonPress) {
      qApp->mainForm()->tabWidget()->showDownloadManager();
    }
  }

  return false;
}

void StatusBar::showProgressDownload(int progress, const QString& tooltip) {
  m_lblProgressDownload->setVisible(true);
  m_barProgressDownload->setVisible(true);
  m_barProgressDownload->setValue(progress);
  m_barProgressDownload->setToolTip(tooltip);
  m_lblProgressDownload->setToolTip(tooltip);
}

void StatusBar::clearProgressDownload() {
  m_lblProgressDownload->setVisible(false);
  m_barProgressDownload->setVisible(false);
  m_barProgressDownload->setValue(0);
}
