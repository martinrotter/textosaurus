// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "saurus/gui/settings/settingsgeneral.h"

#include "common/gui/guiutilities.h"
#include "common/miscellaneous/systemfactory.h"
#include "saurus/miscellaneous/application.h"
#include "saurus/miscellaneous/textapplication.h"
#include "saurus/miscellaneous/textapplicationsettings.h"

SettingsGeneral::SettingsGeneral(Settings* settings, QWidget* parent) : SettingsPanel(settings, parent) {
  m_ui.setupUi(this);
  connect(m_ui.m_checkRemoveTrolltechJunk, &QCheckBox::stateChanged, this, &SettingsGeneral::dirtifySettings);
  connect(m_ui.m_checkRestoreSession, &QCheckBox::stateChanged, this, &SettingsGeneral::dirtifySettings);

  GuiUtilities::setLabelAsNotice(*m_ui.m_lblRestoreSession, false);
}

void SettingsGeneral::loadSettings() {
  onBeginLoadSettings();

#if defined(Q_OS_WIN)
  m_ui.m_checkRemoveTrolltechJunk->setVisible(true);
  m_ui.m_checkRemoveTrolltechJunk->setChecked(settings()->value(GROUP(General), SETTING(General::RemoveTrolltechJunk)).toBool());
#else
  m_ui.m_checkRemoveTrolltechJunk->setVisible(false);
#endif

  m_ui.m_checkRestoreSession->setChecked(qApp->textApplication()->settings()->restorePreviousSession());

  onEndLoadSettings();
}

void SettingsGeneral::saveSettings() {
  onBeginSaveSettings();
  settings()->setValue(GROUP(General), General::RemoveTrolltechJunk, m_ui.m_checkRemoveTrolltechJunk->isChecked());
  qApp->textApplication()->settings()->setRestorePreviousSession(m_ui.m_checkRestoreSession->isChecked());
  onEndSaveSettings();
}
