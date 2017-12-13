// For license of this file, see <project-root-folder>/LICENSE.md.

#include "gui/tabwidget.h"

#include "definitions/definitions.h"
#include "gui/dialogs/formmain.h"
#include "gui/plaintoolbutton.h"
#include "gui/tabbar.h"
#include "gui/texteditor.h"
#include "miscellaneous/application.h"
#include "miscellaneous/iconfactory.h"
#include "miscellaneous/settings.h"
#include "miscellaneous/textfactory.h"

#include <QToolButton>

TabWidget::TabWidget(QWidget* parent) : QTabWidget(parent) {
  setTabBar(new TabBar(this));
  setUsesScrollButtons(true);
  setMovable(true);
  setIconSize(QSize(12, 12));

  createConnections();
}

TabWidget::~TabWidget() {
  qDebug("Destroying TabWidget instance.");
}

void TabWidget::createConnections() {
  connect(tabBar(), &TabBar::tabCloseRequested, this, &TabWidget::closeTab);
}

bool TabWidget::closeTab(int index) {
  return removeTab(index, true);
}

void TabWidget::closeAllTabsExceptCurrent() {
  // Close tabs after active tab.
  int index_of_active = currentIndex();

  for (int i = count() - 1; i >= 0; i--) {
    if (i != index_of_active) {
      if (i < index_of_active) {
        index_of_active--;
      }

      closeTab(i);
    }
  }
}

void TabWidget::closeAllTabs() {
  for (int i = count() - 1; i >= 0; i--) {
    closeTab(i);
  }
}

void TabWidget::gotoNextTab() {
  if (currentIndex() == count() - 1) {
    setCurrentIndex(0);
  }
  else {
    setCurrentIndex(currentIndex() + 1);
  }
}

void TabWidget::gotoPreviousTab() {
  if (currentIndex() == 0) {
    setCurrentIndex(count() - 1);
  }
  else {
    setCurrentIndex(currentIndex() - 1);
  }
}

void TabWidget::indentTabText(int index) {
#if defined (Q_OS_MACOS)
  if (!tabIcon(index).isNull()) {
    // We have closable tab with some icon, fix the title.
    const QString text = tabText(index);

    if (!text.startsWith(QSL("  "))) {
      setTabText(index, QSL("  ") + text);
    }
  }
#else
  Q_UNUSED(index)
#endif
}

bool TabWidget::removeTab(int index, bool clear_from_memory) {
  bool closed = widget(index)->close();

  if (closed) {
    if (clear_from_memory) {
      widget(index)->deleteLater();
    }

    QTabWidget::removeTab(index);
  }

  return closed;
}

TextEditor* TabWidget::textEditorAt(int index) const {
  if (index < 0 || index >= count()) {
    return nullptr;
  }
  else {
    return qobject_cast<TextEditor*>(widget(index));
  }
}

QList<TextEditor*> TabWidget::editors() const {
  QList<TextEditor*> editors;

  for (int i = 0; i < count(); i++) {
    TextEditor* edit = textEditorAt(i);

    if (edit != nullptr) {
      editors.append(edit);
    }
  }

  return editors;
}

int TabWidget::addTab(QWidget* widget, const QIcon& icon, const QString& label, TabBar::TabType type) {
  const int index = QTabWidget::addTab(widget, icon, label);

  tabBar()->setTabType(index, type);
  indentTabText(index);

  return index;
}

int TabWidget::addTab(QWidget* widget, const QString& label, TabBar::TabType type) {
  const int index = QTabWidget::addTab(widget, label);

  tabBar()->setTabType(index, type);
  indentTabText(index);

  return index;
}

int TabWidget::insertTab(int index, QWidget* widget, const QIcon& icon, const QString& label, TabBar::TabType type) {
  const int tab_index = QTabWidget::insertTab(index, widget, icon, label);

  tabBar()->setTabType(tab_index, type);
  indentTabText(index);

  return tab_index;
}

int TabWidget::insertTab(int index, QWidget* widget, const QString& label, TabBar::TabType type) {
  const int tab_index = QTabWidget::insertTab(index, widget, label);

  tabBar()->setTabType(tab_index, type);
  indentTabText(index);

  return tab_index;
}

void TabWidget::changeIcon(int index, const QIcon& new_icon) {
  setTabIcon(index, new_icon);
  indentTabText(index);
}

void TabWidget::changeTitle(int index, const QString& new_title) {
  setTabText(index, new_title);
  setTabToolTip(index, new_title);
  indentTabText(index);
}
