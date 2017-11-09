// For license of this file, see <project-root-folder>/LICENSE.md.

#include "gui/settings/settingsexternaltools.h"

#include "miscellaneous/application.h"
#include "miscellaneous/iconfactory.h"

SettingsExternalTools::SettingsExternalTools(Settings* settings, QWidget* parent)
  : SettingsPanel(settings, parent) {
  m_ui.setupUi(this);
  m_ui.m_shortcut->hideResetButton();
  m_ui.m_btnAdd->setIcon(qApp->icons()->fromTheme(QSL("list-add")));
  m_ui.m_btnDelete->setIcon(qApp->icons()->fromTheme(QSL("list-remove")));
}

void SettingsExternalTools::loadSettings() {
  onBeginLoadSettings();

  onEndLoadSettings();
}

void SettingsExternalTools::saveSettings() {
  onBeginSaveSettings();

  onEndSaveSettings();
}
