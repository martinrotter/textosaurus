// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "saurus/gui/settings/settingseditor.h"

#include "common/miscellaneous/settings.h"
#include "saurus/gui/settings/syntaxcolorthemeeditor.h"
#include "saurus/miscellaneous/application.h"
#include "saurus/miscellaneous/syntaxhighlighting.h"
#include "saurus/miscellaneous/textapplication.h"
#include "saurus/miscellaneous/textapplicationsettings.h"

#include <QFontDatabase>
#include <QFontDialog>

SettingsEditor::SettingsEditor(Settings* settings, QWidget* parent)
  : SettingsPanel(settings, parent) {
  m_ui.setupUi(this);
  m_ui.m_spinLineSpacing->setSuffix(QString(QL1C(' ')) + tr("pixels"));

  m_ui.m_cmbIndentMode->addItem(tr("Spaces"), false);
  m_ui.m_cmbIndentMode->addItem(tr("Tabs"), true);

  connect(m_ui.m_cbReloadFilesAutomatically, &QCheckBox::toggled, this, &SettingsEditor::dirtifySettings);
  connect(m_ui.m_cbEdge, &QCheckBox::toggled, this, &SettingsEditor::dirtifySettings);
  connect(m_ui.m_cmbIndentMode, &QComboBox::currentTextChanged, this, &SettingsEditor::dirtifySettings);
  connect(m_ui.m_cmbTimestampFormat, &QComboBox::currentTextChanged, this, &SettingsEditor::dirtifySettings);
  connect(m_ui.m_cmbDateTimeFormat, &QComboBox::currentTextChanged, this, &SettingsEditor::dirtifySettings);
  connect(m_ui.m_spinEdge, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &SettingsEditor::dirtifySettings);
  connect(m_ui.m_spinIndentSize, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &SettingsEditor::dirtifySettings);
  connect(m_ui.m_spinTabSize, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &SettingsEditor::dirtifySettings);
  connect(m_ui.m_spinLineSpacing, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &SettingsEditor::dirtifySettings);
  connect(m_ui.m_btnFontMainChange, &QPushButton::clicked, this, &SettingsEditor::changeMainFont);
  connect(m_ui.m_themeEditor, &SyntaxColorThemeEditor::colorThemesEdited, this, &SettingsEditor::dirtifySettings);
}

void SettingsEditor::loadSettings() {
  onBeginLoadSettings();

  m_ui.m_spinEdge->setValue(qApp->textApplication()->settings()->edgeLineColumn());
  m_ui.m_cbEdge->setChecked(qApp->textApplication()->settings()->edgeLineEnabled());
  m_ui.m_cbReloadFilesAutomatically->setChecked(qApp->textApplication()->settings()->reloadModifiedDocumentsAutomatically());
  m_ui.m_cmbIndentMode->setCurrentIndex(m_ui.m_cmbIndentMode->findData(qApp->textApplication()->settings()->indentWithTabs()));
  m_ui.m_spinIndentSize->setValue(qApp->textApplication()->settings()->indentSize());
  m_ui.m_spinTabSize->setValue(qApp->textApplication()->settings()->tabSize());
  m_ui.m_lblFontMain->setFont(qApp->textApplication()->settings()->mainFont());
  m_ui.m_spinLineSpacing->setValue(qApp->textApplication()->settings()->lineSpacing());
  m_ui.m_themeEditor->loadColorThemes(qApp->textApplication()->settings()->syntaxHighlighting()->colorThemes(),
                                      qApp->textApplication()->settings()->syntaxHighlighting()->currentColorTheme().name());

  QStringList date_patterns;

  date_patterns << QSL("yyyy-MM-ddTHH:mm:ss") << QSL("MMM dd yyyy hh:mm:ss") <<
    QSL("MMM d yyyy hh:mm:ss") << QSL("ddd, dd MMM yyyy HH:mm:ss") <<
    QSL("dd MMM yyyy") << QSL("yyyy-MM-dd HH:mm:ss.z") << QSL("yyyy-MM-dd") <<
    QSL("yyyy") << QSL("yyyy-MM") << QSL("yyyy-MM-dd") << QSL("yyyy-MM-ddThh:mm") <<
    QSL("yyyy-MM-ddThh:mm:ss");

  for (const QString& pattern : qAsConst(date_patterns)) {
    m_ui.m_cmbTimestampFormat->addItem(pattern);
    m_ui.m_cmbDateTimeFormat->addItem(pattern);
  }

  m_ui.m_cmbTimestampFormat->setCurrentText(qApp->textApplication()->settings()->logTimestampFormat());
  m_ui.m_cmbDateTimeFormat->setCurrentText(qApp->textApplication()->settings()->dateTimeTimestampFormat());

  onEndLoadSettings();
}

void SettingsEditor::saveSettings() {
  onBeginSaveSettings();

  qApp->textApplication()->settings()->blockSignals(true);
  qApp->textApplication()->settings()->setEdgeLineEnabled(m_ui.m_cbEdge->isChecked());
  qApp->textApplication()->settings()->setEdgeLineColumn(m_ui.m_spinEdge->value());
  qApp->textApplication()->settings()->setReloadModifiedDocumentsAutomatically(m_ui.m_cbReloadFilesAutomatically->isChecked());
  qApp->textApplication()->settings()->setIndentSize(m_ui.m_spinIndentSize->value());
  qApp->textApplication()->settings()->setTabSize(m_ui.m_spinTabSize->value());
  qApp->textApplication()->settings()->setIndentWithTabs(m_ui.m_cmbIndentMode->currentData().toBool());
  qApp->textApplication()->settings()->setMainFont(m_ui.m_lblFontMain->font());
  qApp->textApplication()->settings()->setLogTimestampFormat(m_ui.m_cmbTimestampFormat->currentText());
  qApp->textApplication()->settings()->setDateTimeTimestampFormat(m_ui.m_cmbDateTimeFormat->currentText());
  qApp->textApplication()->settings()->syntaxHighlighting()->saveColorThemes(m_ui.m_themeEditor->colorThemes(),
                                                                             m_ui.m_themeEditor->currentColorThemeIndex());

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
