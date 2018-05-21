// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "saurus/gui/tab.h"

#include "saurus/gui/tabwidget.h"

#include <QWidget>

Tab::Tab(TabWidget* tab_widget) : QWidget(tab_widget), m_tabWidget(tab_widget) {}

int Tab::index() const {
  return m_tabWidget == nullptr ? -1 : m_tabWidget->indexOf(dynamic_cast<QWidget*>(const_cast<Tab*>(this)));
}
