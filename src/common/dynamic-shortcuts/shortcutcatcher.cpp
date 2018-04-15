// For license of this file, see <project-root-folder>/LICENSE.md.

#include "common/dynamic-shortcuts/shortcutcatcher.h"

#include "common/gui/plaintoolbutton.h"
#include "common/miscellaneous/iconfactory.h"
#include "saurus/miscellaneous/application.h"

#include <QHBoxLayout>
#include <QKeySequenceEdit>

// TODO: vymyslet co se závislostí na saurus application
ShortcutCatcher::ShortcutCatcher(QWidget* parent) : QWidget(parent) {
  // Setup layout of the control
  m_layout = new QHBoxLayout(this);
  m_layout->setMargin(0);
  m_layout->setSpacing(1);

  // Create reset button.
  m_btnReset = new PlainToolButton(this);
  m_btnReset->setIcon(qApp->icons()->fromTheme(QSL("document-revert")));
  m_btnReset->setFocusPolicy(Qt::NoFocus);
  m_btnReset->setToolTip(tr("Reset to original shortcut."));

  // Create clear button.
  m_btnClear = new PlainToolButton(this);
  m_btnClear->setIcon(qApp->icons()->fromTheme(QSL("list-remove")));
  m_btnClear->setFocusPolicy(Qt::NoFocus);
  m_btnClear->setToolTip(tr("Clear current shortcut."));

  // Clear main shortcut catching button.
  m_btnChange = new QKeySequenceEdit(this);
  m_btnChange->setFocusPolicy(Qt::StrongFocus);
  m_btnChange->setToolTip(tr("Click and hit new shortcut."));

  // Add both buttons to the layout.
  m_layout->addWidget(m_btnChange);
  m_layout->addWidget(m_btnReset);
  m_layout->addWidget(m_btnClear);

  // Establish needed connections.
  connect(m_btnReset, &QToolButton::clicked, this, &ShortcutCatcher::resetShortcut);
  connect(m_btnClear, &QToolButton::clicked, this, &ShortcutCatcher::clearShortcut);
  connect(m_btnChange, &QKeySequenceEdit::keySequenceChanged, this, &ShortcutCatcher::shortcutChanged);
}

QKeySequence ShortcutCatcher::shortcut() const {
  return m_btnChange->keySequence();
}

void ShortcutCatcher::setDefaultShortcut(const QKeySequence& key) {
  m_defaultSequence = key;
  setShortcut(key);
}

void ShortcutCatcher::setShortcut(const QKeySequence& key) {
  m_btnChange->setKeySequence(key);
}

void ShortcutCatcher::hideResetButton() {
  m_btnReset->hide();
}

void ShortcutCatcher::resetShortcut() {
  m_btnChange->setKeySequence(m_defaultSequence);
}

void ShortcutCatcher::clearShortcut() {
  m_btnChange->clear();
}
