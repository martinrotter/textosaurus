// For license of this file, see <project-root-folder>/LICENSE.md.

#include "saurus/gui/tabwidget.h"

#include "common/gui/plaintoolbutton.h"
#include "common/miscellaneous/iconfactory.h"
#include "common/miscellaneous/settings.h"
#include "common/miscellaneous/textfactory.h"
#include "definitions/definitions.h"
#include "saurus/gui/dialogs/formmain.h"
#include "saurus/gui/tab.h"
#include "saurus/gui/tabbar.h"
#include "saurus/gui/texteditor.h"
#include "saurus/miscellaneous/application.h"

#include <QToolButton>

TabWidget::TabWidget(QWidget* parent) : QTabWidget(parent) {
  setTabBar(new TabBar(this));
  setUsesScrollButtons(true);
  setMovable(true);
  setIconSize(QSize(12, 12));
  setDocumentMode(true);

  createConnections();
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

void TabWidget::closeCurrentTab() {
  if (currentIndex() >= 0) {
    closeTab(currentIndex());
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
  auto widg = widget(index);
  bool closed = widg != nullptr && widg->close();

  if (closed) {
    if (clear_from_memory) {
      widg->deleteLater();
    }

    QTabWidget::removeTab(index);
  }

  return closed;
}

TextEditor* TabWidget::currentEditor() const {
  return textEditorAt(currentIndex());
}

Tab* TabWidget::currentTab() const {
  return tabAt(currentIndex());
}

int TabWidget::indexOfEditor(TextEditor* editor) const {
  int i = 0;

  for (Tab* tab : tabs()) {
    if (tab->primaryEditor() == editor) {
      return i;
    }
    else {
      i++;
    }
  }

  return -1;
}

TextEditor* TabWidget::textEditorAt(int index) const {
  Tab* tab = tabAt(index);

  return tab != nullptr ? tab->primaryEditor() : nullptr;
}

Tab* TabWidget::tabAt(int index) const {
  if (index < 0 || index >= count()) {
    return nullptr;
  }
  else {
    return qobject_cast<Tab*>(widget(index));
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

QList<Tab*> TabWidget::tabs() const {
  QList<Tab*> editors;

  for (int i = 0; i < count(); i++) {
    Tab* edit = tabAt(i);

    if (edit != nullptr) {
      editors.append(edit);
    }
  }

  return editors;
}

int TabWidget::addTab(Tab* widget, const QIcon& icon, const QString& label) {
  const int index = QTabWidget::addTab(widget, icon, label);

  tabBar()->setupTabControls(index);
  indentTabText(index);

  return index;
}

int TabWidget::addTab(Tab* widget, const QString& label) {
  const int index = QTabWidget::addTab(widget, label);

  tabBar()->setupTabControls(index);
  indentTabText(index);

  return index;
}

int TabWidget::insertTab(int index, Tab* widget, const QIcon& icon, const QString& label) {
  const int tab_index = QTabWidget::insertTab(index, widget, icon, label);

  tabBar()->setupTabControls(tab_index);
  indentTabText(index);

  return tab_index;
}

int TabWidget::insertTab(int index, Tab* widget, const QString& label) {
  const int tab_index = QTabWidget::insertTab(index, widget, label);

  tabBar()->setupTabControls(tab_index);
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
