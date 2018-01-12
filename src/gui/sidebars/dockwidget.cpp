// For license of this file, see <project-root-folder>/LICENSE.md.

#include "src/gui/sidebars/dockwidget.h"

#include <QEvent>
#include <QMetaObject>

DockWidget::DockWidget(TextApplication* text_app, QWidget* parent) : QDockWidget(parent), m_textApp(text_app) {
  setContentsMargins(0, 0, 0, 0);
  setFeatures(QDockWidget::DockWidgetFeature::DockWidgetClosable | QDockWidget::DockWidgetFeature::DockWidgetMovable);
}

void DockWidget::setTextApplication(TextApplication* text_app) {
  m_textApp = text_app;
}

void DockWidget::switchVisibility() {
  isVisible() ? hide() : show();
}

void DockWidget::showEvent(QShowEvent* event) {
  load();
  QDockWidget::showEvent(event);
}
