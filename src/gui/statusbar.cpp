// For license of this file, see <project-root-folder>/LICENSE.md.

#include "gui/statusbar.h"

StatusBar::StatusBar(QWidget* parent) : QStatusBar(parent) {
  setSizeGripEnabled(false);
  setContentsMargins(2, 0, 2, 2);
}

StatusBar::~StatusBar() {
  qDebug("Destroying StatusBar instance.");
}

bool StatusBar::eventFilter(QObject* watched, QEvent* event) {
  Q_UNUSED(watched)
  Q_UNUSED(event)

  return false;
}
