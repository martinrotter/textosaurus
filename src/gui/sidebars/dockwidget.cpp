// For license of this file, see <project-root-folder>/LICENSE.md.

#include "src/gui/sidebars/dockwidget.h"

#include <QEvent>
#include <QMetaObject>

DockWidget::DockWidget(QWidget* parent) : QDockWidget(parent) {
  setContentsMargins(0, 0, 0, 0);
  setFeatures(QDockWidget::DockWidgetFeature::DockWidgetClosable | QDockWidget::DockWidgetFeature::DockWidgetMovable);
}

void DockWidget::showEvent(QShowEvent* event) {
  load();
  QDockWidget::showEvent(event);
}
