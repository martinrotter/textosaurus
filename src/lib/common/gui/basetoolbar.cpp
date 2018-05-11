// For license of this file, see <project-root-folder>/LICENSE.md.

#include "common/gui/basetoolbar.h"

#include "common/miscellaneous/settings.h"
#include "definitions/definitions.h"

#include <QWidgetAction>

BaseToolBar::BaseToolBar(const QString& title, QWidget* parent) : QToolBar(title, parent) {
  setIconSize(QSize(20, 20));
}

void BaseBar::loadSavedActions() {
  loadSpecificActions(getSpecificActions(savedActions()));
}

QAction* BaseBar::findMatchingAction(const QString& action, const QList<QAction*>& actions) const {
  foreach (QAction* act, actions) {
    if (act->objectName() == action) {
      return act;
    }
  }

  return nullptr;
}
