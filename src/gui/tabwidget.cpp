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
  createConnections();
  checkTabBarVisibility();
  setDocumentMode(true);
  setUsesScrollButtons(false);
}

TabWidget::~TabWidget() {
  qDebug("Destroying TabWidget instance.");
}

void TabWidget::showDownloadManager() {
  for (int i = 0; i < count(); i++) {
    if (tabBar()->tabType(i) == TabBar::TabType::DownloadManager) {
      setCurrentIndex(i);
      return;
    }
  }

  // Download manager is not opened. Create tab with it.
  qApp->downloadManager()->setParent(this);
  addTab(qApp->downloadManager(), qApp->icons()->fromTheme(QSL("emblem-downloads")), tr("Downloads"), TabBar::DownloadManager);
  setCurrentIndex(count() - 1);
}

void TabWidget::checkTabBarVisibility() {
  const bool should_be_bar_visible = count() > 1 || !qApp->settings()->value(GROUP(GUI), SETTING(GUI::HideTabBarIfOnlyOneTab)).toBool();

  tabBar()->setVisible(should_be_bar_visible);
}

void TabWidget::tabInserted(int index) {
  QTabWidget::tabInserted(index);
  checkTabBarVisibility();
}

void TabWidget::tabRemoved(int index) {
  QTabWidget::tabRemoved(index);
  checkTabBarVisibility();
}

void TabWidget::createConnections() {
  connect(tabBar(), &TabBar::tabCloseRequested, this, &TabWidget::closeTab);
}

void TabWidget::setupIcons() {
  // Iterate through all tabs and update icons
  // accordingly.
  for (int index = 0; index < count(); index++) {
    // Index 0 usually contains widget which displays feeds & messages.
    if (tabBar()->tabType(index) == TabBar::TextEditor) {
      setTabIcon(index, qApp->icons()->fromTheme(QSL("application-text")));
    }
  }
}

bool TabWidget::closeTab(int index) {
  if (tabBar()->tabType(index) == TabBar::TabType::Closable || tabBar()->tabType(index) == TabBar::TabType::TextEditor) {
    removeTab(index, true);
    return true;
  }
  else if (tabBar()->tabType(index) == TabBar::TabType::DownloadManager) {
    removeTab(index, false);
    return true;
  }
  else {
    return false;
  }
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
  if (tabBar()->tabType(index) != TabBar::FeedReader && !tabIcon(index).isNull()) {
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

void TabWidget::removeTab(int index, bool clear_from_memory) {
  if (clear_from_memory) {
    widget(index)->deleteLater();
  }

  QTabWidget::removeTab(index);
}

TextEditor* TabWidget::textEditorAt(int index) const {
  return qobject_cast<TextEditor*>(widget(index));
}

int TabWidget::addTab(QWidget* widget, const QIcon& icon, const QString& label, const TabBar::TabType& type) {
  const int index = QTabWidget::addTab(widget, icon, label);

  tabBar()->setTabType(index, type);
  indentTabText(index);

  /*if (count() == 1) {
     emit currentChanged(index);
     }*/

  return index;
}

int TabWidget::addTab(QWidget* widget, const QString& label, const TabBar::TabType& type) {
  const int index = QTabWidget::addTab(widget, label);

  tabBar()->setTabType(index, type);
  indentTabText(index);

  /*if (count() == 1) {
     emit currentChanged(index);
     }*/

  return index;
}

int TabWidget::insertTab(int index, QWidget* widget, const QIcon& icon, const QString& label, const TabBar::TabType& type) {
  const int tab_index = QTabWidget::insertTab(index, widget, icon, label);

  tabBar()->setTabType(tab_index, type);
  indentTabText(index);

  /*if (count() == 1) {
     emit currentChanged(index);
     }*/

  return tab_index;
}

int TabWidget::insertTab(int index, QWidget* widget, const QString& label, const TabBar::TabType& type) {
  const int tab_index = QTabWidget::insertTab(index, widget, label);

  tabBar()->setTabType(tab_index, type);
  indentTabText(index);

  /*if (count() == 1) {
     emit currentChanged(index);
     }*/

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
