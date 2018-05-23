// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "saurus/gui/sidebars/basesidebar.h"

#include "definitions/definitions.h"

#include <QAction>
#include <QEvent>
#include <QMetaObject>

BaseSidebar::BaseSidebar(TextApplication* text_app, QWidget* parent) : QDockWidget(parent), m_textApp(text_app) {
  setContentsMargins(0, 0, 0, 0);
  setFeatures(QDockWidget::DockWidgetFeature::DockWidgetClosable | QDockWidget::DockWidgetFeature::DockWidgetMovable);
}

QAction* BaseSidebar::generateAction() {
  QAction* act_show = new QAction(windowTitle(), this);

  act_show->setObjectName(QString("m_actionShow") + windowTitle().replace(QSL(" "), QSL("")));
  act_show->setCheckable(true);

  connect(this, &BaseSidebar::visibilityChanged, act_show, &QAction::setChecked);
  connect(act_show, &QAction::triggered, this, &BaseSidebar::switchVisibility);

  return act_show;
}

void BaseSidebar::switchVisibility() {
  isVisible() ? hide() : show();
}

void BaseSidebar::load() {
  setSizePolicy(QSizePolicy::Policy::Ignored, QSizePolicy::Policy::Ignored);

  if (widget() != nullptr) {
    widget()->setSizePolicy(QSizePolicy::Policy::Ignored, QSizePolicy::Policy::Ignored);
  }
}

void BaseSidebar::showEvent(QShowEvent* event) {
  load();
  QDockWidget::showEvent(event);
}
