// For license of this file, see <project-root-folder>/LICENSE.md.

#include "src/gui/docks/dockwidget.h"

#include <QMetaObject>

DockWidget::DockWidget(QWidget* parent) : QDockWidget(parent) {
  setContentsMargins(0, 0, 0, 0);
  setFeatures(QDockWidget::DockWidgetFeature::DockWidgetClosable | QDockWidget::DockWidgetFeature::DockWidgetMovable);
}
