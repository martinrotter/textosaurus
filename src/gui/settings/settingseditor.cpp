// For license of this file, see <project-root-folder>/LICENSE.md.

#include "gui/settings/settingseditor.h"

#include "miscellaneous/application.h"
#include "miscellaneous/settings.h"
#include "miscellaneous/textapplication.h"
#include "miscellaneous/textapplicationsettings.h"

#include <QFontDatabase>
#include <QFontDialog>

SettingsEditor::SettingsEditor(Settings* settings, QWidget* parent)
  : SettingsPanel(settings, parent) {
  m_ui.setupUi(this);

  connect(m_ui.m_btnFontMainChange, &QPushButton::clicked, this, &SettingsEditor::changeMainFont);
}

void SettingsEditor::loadSettings() {
  onBeginLoadSettings();

  m_ui.m_lblFontMain->setFont(qApp->textApplication()->settings()->mainFont());

  onEndLoadSettings();
}

void SettingsEditor::saveSettings() {
  onBeginSaveSettings();

  qApp->textApplication()->settings()->setMainFont(m_ui.m_lblFontMain->font());

  onEndSaveSettings();
}

void SettingsEditor::changeMainFont() {
  bool ok;

  m_ui.m_lblFontMain->setFont(QFontDialog::getFont(&ok,
                                                   m_ui.m_lblFontMain->font(),
                                                   parentWidget(),
                                                   tr("Select new text editor main font")));

  if (ok) {
    dirtifySettings();
  }
}
