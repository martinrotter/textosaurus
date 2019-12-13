// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "saurus/gui/dialogs/formsettings.h"

#include "common/gui/guiutilities.h"
#include "common/gui/messagebox.h"
#include "common/miscellaneous/iconfactory.h"
#include "common/miscellaneous/settings.h"
#include "definitions/definitions.h"
#include "saurus/miscellaneous/application.h"

#include "saurus/gui/settings/settingsbrowsermail.h"
#include "saurus/gui/settings/settingseditor.h"
#include "saurus/gui/settings/settingsencryption.h"
#include "saurus/gui/settings/settingsexternaltools.h"
#include "saurus/gui/settings/settingsgeneral.h"
#include "saurus/gui/settings/settingsgui.h"
#include "saurus/gui/settings/settingslocalization.h"
#include "saurus/gui/settings/settingsplugins.h"
#include "saurus/gui/settings/settingsshortcuts.h"

#include <QTimer>

FormSettings::FormSettings(QWidget& parent)
  : QDialog(&parent), m_settings(*qApp->settings()) {
  m_ui.setupUi(this);

  GuiUtilities::applyDialogProperties(*this, qApp->icons()->fromTheme(QSL("document-properties")));

  m_btnApply = m_ui.m_buttonBox->button(QDialogButtonBox::Apply);
  m_btnApply->setEnabled(false);

  // Establish needed connections.
  connect(m_ui.m_buttonBox, &QDialogButtonBox::accepted, this, &FormSettings::saveSettings);
  connect(m_ui.m_buttonBox, &QDialogButtonBox::rejected, this, &FormSettings::cancelSettings);
  connect(m_btnApply, &QPushButton::clicked, this, &FormSettings::applySettings);

  addSettingsPanel(new SettingsGeneral(&m_settings, this));
  addSettingsPanel(new SettingsGui(&m_settings, this));
  addSettingsPanel(new SettingsShortcuts(&m_settings, this));
  addSettingsPanel(new SettingsEditor(&m_settings, this));
  addSettingsPanel(new SettingsEncryption(&m_settings, this));
  addSettingsPanel(new SettingsExternalTools(&m_settings, this));
  addSettingsPanel(new SettingsPlugins(&m_settings, this));
  addSettingsPanel(new SettingsLocalization(&m_settings, this));
  addSettingsPanel(new SettingsBrowserMail(&m_settings, this));

  m_ui.m_listSettings->setCurrentRow(0);

  GuiUtilities::disableCloseButton(*this);
}

void FormSettings::saveSettings() {
  applySettings();
  accept();
}

void FormSettings::applySettings() {
  // Save all settings.
  m_settings.checkSettings();
  QStringList panels_for_restart;

  foreach (SettingsPanel* panel, m_panels) {
    if (panel->isDirty()) {
      panel->saveSettings();
    }

    if (panel->requiresRestart()) {
      panels_for_restart.append(panel->title().toLower());
      panel->setRequiresRestart(false);
    }
  }

  if (!panels_for_restart.isEmpty()) {
    const QStringList changed_settings_description = panels_for_restart.replaceInStrings(QRegExp(QSL("^")), QString::fromUtf8(" • "));
    const QMessageBox::StandardButton clicked_button = MessageBox::show(this,
                                                                        QMessageBox::Question,
                                                                        tr("Critical Settings Were Changed"),
                                                                        tr(
                                                                          "Some critical settings were changed and will be applied after the application gets restarted. "
                                                                          "\n\nYou have to restart manually."),
                                                                        tr("Do you want to restart now?"),
                                                                        tr("Changed categories of settings:\n%1.").arg(
                                                                          changed_settings_description.join(QSL(",\n"))),
                                                                        QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

    if (clicked_button == QMessageBox::Yes) {
      close();
      QTimer::singleShot(200, qApp, &Application::restart);
    }
  }

  m_btnApply->setEnabled(false);
}

void FormSettings::cancelSettings() {
  QStringList changed_panels;

  foreach (SettingsPanel* panel, m_panels) {
    if (panel->isDirty()) {
      changed_panels.append(panel->title().toLower());
    }
  }

  if (changed_panels.isEmpty()) {
    reject();
  }
  else {
    const QStringList changed_settings_description = changed_panels.replaceInStrings(QRegExp(QSL("^")), QString::fromUtf8(" • "));

    if (MessageBox::show(this,
                         QMessageBox::Critical,
                         tr("Some Settings are Changed and Will be Lost"),
                         tr("Some settings were changed and by cancelling this dialog, you would lose these changes."),
                         tr("Do you really want to close this dialog without saving any settings?"),
                         tr("Changed categories of settings:\n%1.").arg(changed_settings_description.join(QSL(",\n"))),
                         QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) ==
        QMessageBox::Yes) {
      reject();
    }
  }
}

void FormSettings::addSettingsPanel(SettingsPanel* panel) {
  m_ui.m_listSettings->addItem(panel->title());
  m_panels.append(panel);
  m_ui.m_stackedSettings->addWidget(panel);
  panel->loadSettings();
  connect(panel, &SettingsPanel::settingsChanged, this, [this]() {
    m_btnApply->setEnabled(true);
  });
}
