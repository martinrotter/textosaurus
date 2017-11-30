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
  m_ui.m_spinLineSpacing->setSuffix(QString(QL1C(' ')) + tr("pixels"));

  connect(m_ui.m_spinLineSpacing, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &SettingsEditor::dirtifySettings);
  connect(m_ui.m_btnFontMainChange, &QPushButton::clicked, this, &SettingsEditor::changeMainFont);
}

void SettingsEditor::loadSettings() {
  onBeginLoadSettings();

  m_ui.m_lblFontMain->setFont(qApp->textApplication()->settings()->mainFont());
  m_ui.m_spinLineSpacing->setValue(qApp->textApplication()->settings()->lineSpacing());

  onEndLoadSettings();
}

void SettingsEditor::saveSettings() {
  onBeginSaveSettings();

  qApp->textApplication()->settings()->blockSignals(true);
  qApp->textApplication()->settings()->setMainFont(m_ui.m_lblFontMain->font());
  qApp->textApplication()->settings()->blockSignals(false);

  // NOTE: Execute last command with signals ON, so that editor do refresh their settings.
  qApp->textApplication()->settings()->setLineSpacing(m_ui.m_spinLineSpacing->value());

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
