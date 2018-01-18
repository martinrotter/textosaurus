// For license of this file, see <project-root-folder>/LICENSE.md.

#include "src/gui/sidebars/basesidebar.h"

#include "definitions/definitions.h"

#include <QAction>
#include <QEvent>
#include <QMetaObject>

BaseSidebar::BaseSidebar(TextApplication* text_app, QWidget* parent) : QDockWidget(parent), m_textApp(text_app) {
  setContentsMargins(0, 0, 0, 0);
  setFeatures(QDockWidget::DockWidgetFeature::DockWidgetClosable | QDockWidget::DockWidgetFeature::DockWidgetMovable);
}

void BaseSidebar::setTextApplication(TextApplication* text_app) {
  m_textApp = text_app;
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

void BaseSidebar::showEvent(QShowEvent* event) {
  load();
  QDockWidget::showEvent(event);
}
