// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "common/gui/basetoolbar.h"

BaseToolBar::BaseToolBar(const QString& title, QWidget* parent) : QToolBar(title, parent) {
  setIconSize(QSize(18, 18));
}

void BaseBar::loadSavedActions() {
  loadSpecificActions(getSpecificActions(savedActions()));
}

QAction* BaseBar::findMatchingAction(const QString& action, const QList<QAction*>& actions) const {
  for (QAction* act : actions) {
    if (act->objectName() == action) {
      return act;
    }
  }

  return nullptr;
}
