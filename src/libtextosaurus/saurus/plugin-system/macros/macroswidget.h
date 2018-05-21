// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifndef MACROSWIDGET_H
#define MACROSWIDGET_H

#include <QWidget>

#include "saurus/plugin-system/macros/macrossidebar.h"

#include "ui_macroswidget.h"

class MacrosWidget : public QWidget {
  Q_OBJECT

  friend class MacrosSidebar;

  public:
    explicit MacrosWidget(QWidget* parent = nullptr);

  private:
    Ui::MacrosWidget m_ui;
};

#endif // MACROSWIDGET_H
