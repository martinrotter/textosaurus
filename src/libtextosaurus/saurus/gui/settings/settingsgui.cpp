// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "saurus/gui/settings/settingsgui.h"

#include "common/gui/systemtrayicon.h"
#include "common/gui/toolbar.h"
#include "common/gui/toolbareditor.h"
#include "common/miscellaneous/iconfactory.h"
#include "common/miscellaneous/settings.h"
#include "saurus/gui/dialogs/formmain.h"
#include "saurus/gui/tabwidget.h"
#include "saurus/miscellaneous/application.h"

#include <QDropEvent>
#include <QStyleFactory>

SettingsGui::SettingsGui(Settings* settings, QWidget* parent) : SettingsPanel(settings, parent) {
  m_ui.setupUi(this);
  m_ui.m_editorToolBar->activeItemsWidget()->viewport()->installEventFilter(this);
  m_ui.m_editorToolBar->availableItemsWidget()->viewport()->installEventFilter(this);

  // Setup skins.
  connect(m_ui.m_cmbIconTheme, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &SettingsGui::requireRestart);
  connect(m_ui.m_cmbIconTheme, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
          &SettingsGui::dirtifySettings);
  connect(m_ui.m_checkCloseTabsDoubleClick, &QCheckBox::toggled, this, &SettingsGui::dirtifySettings);
  connect(m_ui.m_checkCloseTabsMiddleClick, &QCheckBox::toggled, this, &SettingsGui::dirtifySettings);
  connect(m_ui.m_checkNewTabDoubleClick, &QCheckBox::toggled, this, &SettingsGui::dirtifySettings);
  connect(m_ui.m_checkHideOnClose, &QCheckBox::toggled, this, &SettingsGui::dirtifySettings);
  connect(m_ui.m_checkBalloonNotifications, &QCheckBox::toggled, this, &SettingsGui::dirtifySettings);
  connect(m_ui.m_checkHideWhenMinimized, &QCheckBox::toggled, this, &SettingsGui::dirtifySettings);
  connect(m_ui.m_grpTrayIcon, &QGroupBox::toggled, this, &SettingsGui::dirtifySettings);
  connect(m_ui.m_grbCloseTabs, &QGroupBox::toggled, this, &SettingsGui::dirtifySettings);
  connect(m_ui.m_cmbToolbarButtonStyle, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
          &SettingsGui::dirtifySettings);
  connect(m_ui.m_editorToolBar, &ToolBarEditor::setupChanged, this, &SettingsGui::dirtifySettings);
  connect(m_ui.m_listStyles, &QListWidget::currentItemChanged, this, &SettingsGui::dirtifySettings);
  connect(m_ui.m_cmbSelectToolBar, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), m_ui.m_stackedToolbars,
          &QStackedWidget::setCurrentIndex);
}

bool SettingsGui::eventFilter(QObject* obj, QEvent* e) {
  Q_UNUSED(obj)

  if (e->type() == QEvent::Drop) {
    QDropEvent* drop_event = dynamic_cast<QDropEvent*>(e);

    if (drop_event != nullptr && drop_event->keyboardModifiers() != Qt::NoModifier) {
      drop_event->setDropAction(Qt::MoveAction);
    }
  }

  return false;
}

void SettingsGui::loadSettings() {
  onBeginLoadSettings();

  // Load settings of icon theme.
  const QString current_theme = settings()->value(GROUP(GUI), SETTING(GUI::IconTheme)).toString();

  foreach (const QString& icon_theme_name, qApp->icons()->installedIconThemes()) {
    if (icon_theme_name == APP_NO_THEME) {
      // Add just "no theme" on other systems.
      m_ui.m_cmbIconTheme->addItem(tr("no icon theme/system icon theme"), APP_NO_THEME);
    }
    else {
      m_ui.m_cmbIconTheme->addItem(icon_theme_name, icon_theme_name);
    }
  }

  // Mark active theme.
  if (current_theme == APP_NO_THEME) {
    // Because "no icon theme" lies at the index 0.
    m_ui.m_cmbIconTheme->setCurrentIndex(0);
  }
  else {
    m_ui.m_cmbIconTheme->setCurrentText(current_theme);
  }

  // Load settings of tray icon.
  if (SystemTrayIcon::isSystemTrayAvailable()) {
    m_ui.m_grpTrayIcon->setChecked(settings()->value(GROUP(GUI), SETTING(GUI::UseTrayIcon)).toBool());
    m_ui.m_checkBalloonNotifications->setChecked(settings()->value(GROUP(GUI), SETTING(GUI::EnableNotifications)).toBool());
    m_ui.m_checkHideWhenMinimized->setChecked(settings()->value(GROUP(GUI), SETTING(GUI::HideMainWindowWhenMinimized)).toBool());
    m_ui.m_checkHideOnClose->setChecked(settings()->value(GROUP(GUI), SETTING(GUI::HideMainWindowWhenClosed)).toBool());
  }
  else {
    // Tray icon is not supported on this machine.
    m_ui.m_grpTrayIcon->setTitle(m_ui.m_grpTrayIcon->title() + QL1C(' ') + tr("(Tray icon is not available.)"));
    m_ui.m_grpTrayIcon->setChecked(false);
    m_ui.m_grpTrayIcon->setEnabled(false);
  }

  // Load styles.
  foreach (const QString& style_name, QStyleFactory::keys()) {
    m_ui.m_listStyles->addItem(style_name);
  }

  QList<QListWidgetItem*> items = m_ui.m_listStyles->findItems(settings()->value(GROUP(GUI), SETTING(GUI::Style)).toString(),
                                                               Qt::MatchFixedString);

  if (!items.isEmpty()) {
    m_ui.m_listStyles->setCurrentItem(items.at(0));
  }

  // Load tab settings.
  m_ui.m_checkCloseTabsMiddleClick->setChecked(settings()->value(GROUP(GUI), SETTING(GUI::TabCloseMiddleClick)).toBool());
  m_ui.m_checkCloseTabsDoubleClick->setChecked(settings()->value(GROUP(GUI), SETTING(GUI::TabCloseDoubleClick)).toBool());
  m_ui.m_checkNewTabDoubleClick->setChecked(settings()->value(GROUP(GUI), SETTING(GUI::TabNewDoubleClick)).toBool());

  // Load toolbar button style.
  m_ui.m_cmbToolbarButtonStyle->addItem(tr("Icon only"), Qt::ToolButtonIconOnly);
  m_ui.m_cmbToolbarButtonStyle->addItem(tr("Text only"), Qt::ToolButtonTextOnly);
  m_ui.m_cmbToolbarButtonStyle->addItem(tr("Text beside icon"), Qt::ToolButtonTextBesideIcon);
  m_ui.m_cmbToolbarButtonStyle->addItem(tr("Text under icon"), Qt::ToolButtonTextUnderIcon);
  m_ui.m_cmbToolbarButtonStyle->addItem(tr("Follow OS style"), Qt::ToolButtonFollowStyle);
  m_ui.m_cmbToolbarButtonStyle->setCurrentIndex(m_ui.m_cmbToolbarButtonStyle->findData(settings()->value(GROUP(GUI),
                                                                                                         SETTING(
                                                                                                           GUI::ToolbarStyle)).toInt()));

  // Load toolbars.
  m_ui.m_editorToolBar->loadFromToolBar(qApp->mainForm()->toolBar());
  onEndLoadSettings();
}

void SettingsGui::saveSettings() {
  onBeginSaveSettings();

  // Save toolbar.
  settings()->setValue(GROUP(GUI), GUI::ToolbarStyle,
                       m_ui.m_cmbToolbarButtonStyle->itemData(m_ui.m_cmbToolbarButtonStyle->currentIndex()));

  // Save tray icon.
  if (SystemTrayIcon::isSystemTrayAvailable()) {
    settings()->setValue(GROUP(GUI), GUI::UseTrayIcon, m_ui.m_grpTrayIcon->isChecked());
    settings()->setValue(GROUP(GUI), GUI::EnableNotifications, m_ui.m_checkBalloonNotifications->isChecked());
    settings()->setValue(GROUP(GUI), GUI::HideMainWindowWhenMinimized, m_ui.m_checkHideWhenMinimized->isChecked());
    settings()->setValue(GROUP(GUI), GUI::HideMainWindowWhenClosed, m_ui.m_checkHideOnClose->isChecked());

    if (m_ui.m_grpTrayIcon->isChecked()) {
      qApp->showTrayIcon();
    }
    else {
      qApp->deleteTrayIcon();
    }
  }

  // Save selected icon theme.
  QString selected_icon_theme = m_ui.m_cmbIconTheme->itemData(m_ui.m_cmbIconTheme->currentIndex()).toString();
  QString original_icon_theme = qApp->icons()->currentIconTheme();

  qApp->settings()->setValue(GROUP(GUI), GUI::IconTheme, selected_icon_theme);

  // Check if icon theme was changed.
  if (selected_icon_theme != original_icon_theme) {
    requireRestart();
  }

  // Set new style.
  if (!m_ui.m_listStyles->selectedItems().isEmpty()) {
    const QString new_style = m_ui.m_listStyles->currentItem()->text();
    const QString old_style = qApp->settings()->value(GROUP(GUI), SETTING(GUI::Style)).toString();

    if (old_style != new_style) {
      requireRestart();
    }

    qApp->settings()->setValue(GROUP(GUI), GUI::Style, new_style);
  }

  // Save tab settings.
  settings()->setValue(GROUP(GUI), GUI::TabCloseMiddleClick, m_ui.m_checkCloseTabsMiddleClick->isChecked());
  settings()->setValue(GROUP(GUI), GUI::TabCloseDoubleClick, m_ui.m_checkCloseTabsDoubleClick->isChecked());
  settings()->setValue(GROUP(GUI), GUI::TabNewDoubleClick, m_ui.m_checkNewTabDoubleClick->isChecked());
  m_ui.m_editorToolBar->saveToolBar();

  qApp->mainForm()->toolBar()->refreshVisualProperties();

  onEndSaveSettings();
}
