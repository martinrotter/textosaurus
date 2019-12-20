// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "saurus/gui/settings/syntaxcolorthemeeditor.h"

#include "common/gui/guiutilities.h"
#include "saurus/gui/texteditor.h"
#include "saurus/miscellaneous/application.h"
#include "saurus/miscellaneous/textapplication.h"

#include <QColorDialog>
#include <QComboBox>
#include <QFormLayout>
#include <QInputDialog>
#include <QMetaEnum>
#include <QPushButton>

SyntaxColorThemeEditor::SyntaxColorThemeEditor(QWidget* parent)
  : QWidget(parent) {
  m_ui.setupUi(this);

  GuiUtilities::setLabelAsNotice(*m_ui.m_lblInfo, true);

  QMap<SyntaxColorTheme::StyleComponents, QString> labels;

  labels[SyntaxColorTheme::StyleComponents::Default] = tr("Base color (fg color only)");
  labels[SyntaxColorTheme::StyleComponents::Comment] = tr("Comments (both oneline and multiline)");
  labels[SyntaxColorTheme::StyleComponents::Number] = tr("Numbers");
  labels[SyntaxColorTheme::StyleComponents::String] = tr("Strings and character literals");
  labels[SyntaxColorTheme::StyleComponents::Operator] = tr("Operators");
  labels[SyntaxColorTheme::StyleComponents::Identifier] = tr("Identifiers");
  labels[SyntaxColorTheme::StyleComponents::Keyword] = tr("Keywords");
  labels[SyntaxColorTheme::StyleComponents::Keyword2] = tr("Secondary keywords");
  labels[SyntaxColorTheme::StyleComponents::Preprocessor] = tr("Preprocessor directives");
  labels[SyntaxColorTheme::StyleComponents::Regex] = tr("Regular expressions");
  labels[SyntaxColorTheme::StyleComponents::Script] = tr("Foreign scripts");
  labels[SyntaxColorTheme::StyleComponents::PlainData] = tr("Plain data");
  labels[SyntaxColorTheme::StyleComponents::Error] = tr("Errors");
  labels[SyntaxColorTheme::StyleComponents::ScintillaMargin] = tr("Margin (line numbers, etc.)");

  //labels[SyntaxColorTheme::StyleComponents::ScintillaBraceLight] = tr("Matching brace");
  //labels[SyntaxColorTheme::StyleComponents::ScintillaBraceBad] = tr("Matching brace (with error)");
  labels[SyntaxColorTheme::StyleComponents::ScintillaControlChar] = tr("Control characters");
  labels[SyntaxColorTheme::StyleComponents::ScintillaIndentGuide] = tr("Indent guides");
  labels[SyntaxColorTheme::StyleComponents::ScintillaUrlHighlight] = tr("URL highlighting");
  labels[SyntaxColorTheme::StyleComponents::ScintillaOccurrencesHighlight] = tr("Occurrences highlighting");

  //labels[SyntaxColorTheme::StyleComponents::ScintillaCallTip] = tr("Calltips");
  //labels[SyntaxColorTheme::StyleComponents::ScintillaFoldDisplayText] = tr("Fold display text");
  labels[SyntaxColorTheme::StyleComponents::ScintillaPaper] = tr("Paper background (bg color only)");
  labels[SyntaxColorTheme::StyleComponents::ScintillaCurrentLine] = tr("Current line background (bg color only)");
  labels[SyntaxColorTheme::StyleComponents::ScintillaEdge] = tr("Color of long lines edge (fg color only)");

  QMetaEnum enums = QMetaEnum::fromType<SyntaxColorTheme::StyleComponents>();

  m_ui.m_listComponents->clear();

  for (int i = 0; i < enums.keyCount(); i++) {
    auto component = SyntaxColorTheme::StyleComponents(enums.value(i));
    auto* item = new QListWidgetItem(m_ui.m_listComponents);

    item->setData(Qt::UserRole, int(component));
    item->setText(labels[component]);

    m_indexMapping[component] = i;
  }

  connect(m_ui.m_cmbThemes, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
          this, &SyntaxColorThemeEditor::onThemeSwitched);
  connect(m_ui.m_listComponents, &QListWidget::currentRowChanged, this, &SyntaxColorThemeEditor::displayComponentDetails);
  connect(m_ui.m_btnCopy, &QPushButton::clicked, this, &SyntaxColorThemeEditor::copyExistingTheme);
  connect(m_ui.m_cbBold, &QCheckBox::clicked, this, &SyntaxColorThemeEditor::updateCurrentComponent);
  connect(m_ui.m_cbItalic, &QCheckBox::clicked, this, &SyntaxColorThemeEditor::updateCurrentComponent);
  connect(m_ui.m_cbUnderline, &QCheckBox::clicked, this, &SyntaxColorThemeEditor::updateCurrentComponent);
  connect(m_ui.m_btnForegroundSample, &PlainToolButton::clicked, this, &SyntaxColorThemeEditor::editForegroundColor);
  connect(m_ui.m_btnBackgroundSample, &PlainToolButton::clicked, this, &SyntaxColorThemeEditor::editBackgroundColor);
  connect(m_ui.m_btnForeground, &QPushButton::clicked, this, &SyntaxColorThemeEditor::clearForegroundColor);
  connect(m_ui.m_btnBackground, &QPushButton::clicked, this, &SyntaxColorThemeEditor::clearBackgroundColor);
  connect(m_ui.m_gbCustomizer, &QGroupBox::clicked, this, &SyntaxColorThemeEditor::updateCurrentComponent);
  connect(m_ui.m_btnDelete, &QPushButton::clicked, this, &SyntaxColorThemeEditor::deleteSelectedTheme);
}

void SyntaxColorThemeEditor::loadColorThemes(const QList<SyntaxColorTheme>& themes, const QString& current_theme_name) {
  m_colorThemes.clear();
  m_ui.m_cmbThemes->clear();

  for (const auto& theme : themes) {
    loadColorTheme(theme);

    if (m_colorThemes.constLast().name() == current_theme_name) {
      m_ui.m_cmbThemes->setCurrentIndex(m_ui.m_cmbThemes->count() - 1);
    }
  }
}

void SyntaxColorThemeEditor::loadColorTheme(const SyntaxColorTheme& theme) {
  m_colorThemes.append(theme);
  m_ui.m_cmbThemes->addItem(theme.predefined() ?
                            tr("%1 (built-in theme)").arg(theme.name()) :
                            tr("%1 (custom theme)").arg(theme.name()), theme.name());
}

QList<SyntaxColorTheme> SyntaxColorThemeEditor::colorThemes() const {
  return m_colorThemes;
}

SyntaxColorTheme& SyntaxColorThemeEditor::currentColorTheme() {
  return m_colorThemes[m_ui.m_cmbThemes->currentIndex()];
}

int SyntaxColorThemeEditor::currentColorThemeIndex() const {
  return m_ui.m_cmbThemes->currentIndex();
}

void SyntaxColorThemeEditor::deleteSelectedTheme() {
  m_colorThemes.removeAt(m_ui.m_cmbThemes->currentIndex());
  m_ui.m_cmbThemes->removeItem(m_ui.m_cmbThemes->currentIndex());
}

void SyntaxColorThemeEditor::editForegroundColor() {
  auto new_color = QColorDialog::getColor(m_ui.m_btnForegroundSample->explicitColor(), this,
                                          tr("Select Foreground Color"), QColorDialog::ColorDialogOption::DontUseNativeDialog);

  if (new_color.isValid()) {
    m_ui.m_btnForegroundSample->setExplicitColor(new_color);
    updateCurrentComponent();
  }
}

void SyntaxColorThemeEditor::clearForegroundColor() {
  m_ui.m_btnForegroundSample->setExplicitColor(QColor());
  updateCurrentComponent();
}

void SyntaxColorThemeEditor::editBackgroundColor() {
  auto new_color = QColorDialog::getColor(m_ui.m_btnBackgroundSample->explicitColor(), this,
                                          tr("Select Background Color"), QColorDialog::ColorDialogOption::DontUseNativeDialog);

  if (new_color.isValid()) {
    m_ui.m_btnBackgroundSample->setExplicitColor(new_color);
    updateCurrentComponent();
  }
}

void SyntaxColorThemeEditor::clearBackgroundColor() {
  m_ui.m_btnBackgroundSample->setExplicitColor(QColor());
  updateCurrentComponent();
}

void SyntaxColorThemeEditor::updateCurrentComponent() {
  SyntaxColorTheme& curr_theme = currentColorTheme();

  SyntaxColorTheme::StyleComponents comp = itemComponent(m_ui.m_listComponents->currentRow());

  if (!m_ui.m_gbCustomizer->isCheckable() || m_ui.m_gbCustomizer->isChecked()) {
    SyntaxColorThemeComponent new_component = generateNewComponent();

    curr_theme.setComponent(comp, new_component);
  }
  else {
    curr_theme.removeComponent(comp);
  }

  emit colorThemesEdited();
}

void SyntaxColorThemeEditor::copyExistingTheme() {
  const QString new_name = QInputDialog::getText(this, tr("Enter name for new theme"), tr("Enter name for new color theme"));

  if (!new_name.isEmpty()) {
    SyntaxColorTheme copy(currentColorTheme());

    copy.setName(new_name);
    copy.setPredefined(false);

    loadColorTheme(copy);
    m_ui.m_cmbThemes->setCurrentIndex(m_ui.m_cmbThemes->count() - 1);

    emit colorThemesEdited();
  }
}

void SyntaxColorThemeEditor::onThemeSwitched(int row) {
  Q_UNUSED(row)

  m_ui.m_listComponents->setCurrentRow(-1);
  m_ui.m_listComponents->setCurrentRow(0);
  m_ui.m_btnDelete->setEnabled(!currentColorTheme().predefined());

  emit colorThemesEdited();
}

void SyntaxColorThemeEditor::displayComponentDetails(int row) {
  if (row >= 0) {
    SyntaxColorTheme curr_theme = currentColorTheme();

    SyntaxColorTheme::StyleComponents component = itemComponent(row);

    if (curr_theme.hasComponent(component)) {
      auto substyle = curr_theme.component(component);

      m_ui.m_btnForegroundSample->setExplicitColor(substyle.m_colorForeground);
      m_ui.m_btnBackgroundSample->setExplicitColor(substyle.m_colorBackground);
      m_ui.m_cbItalic->setChecked(substyle.m_italicFont);
      m_ui.m_cbUnderline->setChecked(substyle.m_underlinedFont);
      m_ui.m_cbBold->setChecked(substyle.m_boldFont);
    }
    else {
      m_ui.m_btnForegroundSample->setExplicitColor(QColor());
      m_ui.m_btnBackgroundSample->setExplicitColor(QColor());
      m_ui.m_cbItalic->setChecked(false);
      m_ui.m_cbUnderline->setChecked(false);
      m_ui.m_cbBold->setChecked(false);
    }

    m_ui.m_gbCustomizer->setCheckable(component != SyntaxColorTheme::StyleComponents::Default);
    m_ui.m_gbCustomizer->setChecked(curr_theme.hasComponent(component));
    m_ui.m_gbCustomizer->setDisabled(curr_theme.predefined());
  }
}

SyntaxColorThemeComponent SyntaxColorThemeEditor::generateNewComponent() const {
  SyntaxColorThemeComponent compon(m_ui.m_btnForegroundSample->explicitColor(),
                                   m_ui.m_btnBackgroundSample->explicitColor(),
                                   m_ui.m_cbBold->isChecked(),
                                   m_ui.m_cbItalic->isChecked(),
                                   m_ui.m_cbUnderline->isChecked());

  return compon;
}

SyntaxColorTheme::StyleComponents SyntaxColorThemeEditor::itemComponent(int row) const {
  return SyntaxColorTheme::StyleComponents(m_ui.m_listComponents->item(row)->data(Qt::ItemDataRole::UserRole).toInt());
}
