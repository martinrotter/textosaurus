// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "saurus/gui/tabbar.h"

#include "common/gui/plaintoolbutton.h"
#include "common/miscellaneous/iconfactory.h"
#include "common/miscellaneous/settings.h"
#include "definitions/definitions.h"
#include "saurus/miscellaneous/application.h"

#include <QMouseEvent>
#include <QStyle>

TabBar::TabBar(QWidget* parent) : QTabBar(parent) {
  setContextMenuPolicy(Qt::CustomContextMenu);
}

void TabBar::setupTabControls(int index) {
  auto button_position = static_cast<ButtonPosition>(style()->styleHint(QStyle::SH_TabBar_CloseButtonPosition,
                                                                        nullptr,
                                                                        this));
  PlainToolButton* close_button = new PlainToolButton(this);

  close_button->setIcon(qApp->icons()->fromTheme(QSL("window-close")));
  close_button->setToolTip(tr("Close This Tab"));
  close_button->setText(tr("Close Tab"));
  close_button->setFixedSize(iconSize());

  // Close underlying tab when button is clicked.
  connect(close_button, &PlainToolButton::clicked, this, &TabBar::closeTabViaButton);
  setTabButton(index, button_position, close_button);
}

void TabBar::closeTabViaButton() {
  const QAbstractButton* close_button = qobject_cast<QAbstractButton*>(sender());
  const QTabBar::ButtonPosition button_position = static_cast<ButtonPosition>(style()->styleHint(QStyle::SH_TabBar_CloseButtonPosition,
                                                                                                 nullptr,
                                                                                                 this));

  if (close_button != nullptr) {
    // Find index of tab for this close button.
    for (int i = 0; i < count(); i++) {
      if (tabButton(i, button_position) == close_button) {
        emit tabCloseRequested(i);

        return;
      }
    }
  }
}

void TabBar::wheelEvent(QWheelEvent* event) {
  const int current_index = currentIndex();
  const int number_of_tabs = count();

  // Make sure rotating works.
  if (number_of_tabs > 1) {
    if (event->delta() > 0) {
      // Scroll to the LEFT tab.
      setCurrentIndex(current_index == 0 ?
                      number_of_tabs - 1 :
                      current_index - 1);
    }
    else if (event->delta() < 0) {
      // Scroll to the RIGHT tab.
      setCurrentIndex(current_index == number_of_tabs - 1 ?
                      0 :
                      current_index + 1);
    }
  }
}

void TabBar::mousePressEvent(QMouseEvent* event) {
  QTabBar::mousePressEvent(event);
  const int tab_index = tabAt(event->pos());

  // Check if user clicked on some tab or on empty space.
  if (tab_index >= 0) {
    // Check if user clicked tab with middle button.
    // NOTE: This needs to be done here because destination does not know the original event.
    if (event->button() & Qt::MiddleButton && qApp->settings()->value(GROUP(GUI), SETTING(GUI::TabCloseMiddleClick)).toBool()) {
      // This tab is closable, so we can close it.
      emit tabCloseRequested(tab_index);
    }
  }
}

void TabBar::mouseDoubleClickEvent(QMouseEvent* event) {
  QTabBar::mouseDoubleClickEvent(event);

  const int tab_index = tabAt(event->pos());

  // Check if user clicked on some tab or on empty space.
  if (tab_index >= 0) {
    // Check if user clicked tab with middle button.
    // NOTE: This needs to be done here because destination does not know the original event.
    if (event->button() & Qt::MouseButton::LeftButton && qApp->settings()->value(GROUP(GUI), SETTING(GUI::TabCloseDoubleClick)).toBool()) {
      // This tab is closable, so we can close it.
      emit tabCloseRequested(tab_index);
    }
  }
  else if (event->button() == Qt::MouseButton::LeftButton) {
    emit emptySpaceDoubleClicked();
  }
}
