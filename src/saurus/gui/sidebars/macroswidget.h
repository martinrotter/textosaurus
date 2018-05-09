// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef MACROSWIDGET_H
#define MACROSWIDGET_H

#include <QWidget>

#include "saurus/gui/sidebars/macrossidebar.h"

#include "ui_macroswidget.h"

namespace Ui {
  class MacrosWidget;
}

class MacrosWidget : public QWidget {
  Q_OBJECT

  friend class MacrosSidebar;

  public:
    explicit MacrosWidget(QWidget* parent = nullptr);

  private:
    Ui::MacrosWidget m_ui;
};

#endif // MACROSWIDGET_H
