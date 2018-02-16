// For license of this file, see <project-root-folder>/LICENSE.md.

#include "miscellaneous/syntaxcolortheme.h"

#include "definitions/definitions.h"
#include "gui/texteditor.h"

#include <QMetaEnum>
#include <QSettings>

#include <utility>

SyntaxColorTheme::SyntaxColorTheme(const QString& name, bool predefined, const QMap<StyleComponents, SyntaxColorThemeComponent>& styles)
  : m_name(name), m_styleColors(styles), m_predefined(predefined) {}

SyntaxColorTheme::SyntaxColorTheme(const SyntaxColorTheme& another)
  : m_styleColors(another.m_styleColors), m_name(another.m_name), m_predefined(another.predefined()) {}

SyntaxColorTheme::SyntaxColorTheme(SyntaxColorTheme&& another)
  : m_styleColors(std::move(another.m_styleColors)), m_name(std::move(another.m_name)), m_predefined(another.predefined()) {}

SyntaxColorThemeComponent SyntaxColorTheme::component(StyleComponents code) {
  return m_styleColors.value(code);
}

SyntaxColorThemeComponent SyntaxColorTheme::randomizedComponent(int scintilla_code) {
  auto colors = m_styleColors.values();

  return colors.at(scintilla_code % colors.size());
}

void SyntaxColorTheme::clearStyleColors() {
  m_styleColors.clear();
}

void SyntaxColorTheme::toFile(const QString& file_path) {
  QSettings settings(file_path, QSettings::Format::IniFormat);
  QMetaEnum enum_converter = QMetaEnum::fromType<SyntaxColorTheme::StyleComponents>();

  QMapIterator<StyleComponents, SyntaxColorThemeComponent> i(m_styleColors);

  while (i.hasNext()) {
    i.next();

    auto component = enum_converter.valueToKey(int(i.key()));
    auto style = i.value();

    settings.beginGroup(component);

    if (style.m_colorForeground.isValid()) {
      settings.setValue(QSL("fore"), style.m_colorForeground.name());
    }
    else {
      settings.remove(QSL("fore"));
    }

    if (style.m_colorBackground.isValid()) {
      settings.setValue(QSL("back"), style.m_colorBackground.name());
    }
    else {
      settings.remove(QSL("back"));
    }

    settings.setValue(QSL("bold"), style.m_boldFont);
    settings.setValue(QSL("italic"), style.m_italicFont);
    settings.setValue(QSL("underlined"), style.m_underlinedFont);

    settings.endGroup();
  }

  settings.sync();
}

SyntaxColorTheme SyntaxColorTheme::fromFile(const QString& file_path) {
  QSettings settings(file_path, QSettings::Format::IniFormat);
  SyntaxColorTheme theme(settings.value(QSL("name")).toString(), false);
  QMetaEnum enum_converter = QMetaEnum::fromType<SyntaxColorTheme::StyleComponents>();

  for (const QString& group : settings.childGroups()) {
    auto component = static_cast<SyntaxColorTheme::StyleComponents>(enum_converter.keyToValue(qPrintable(group)));

    settings.beginGroup(group);
    theme.m_styleColors.insert(component, SyntaxColorThemeComponent(settings.value(QSL("fore")).toString(),
                                                                    settings.value(QSL("back")).toString(),
                                                                    settings.value(QSL("bold")).toBool(),
                                                                    settings.value(QSL("italic")).toBool(),
                                                                    settings.value(QSL("underlined")).toBool()));
    settings.endGroup();
  }

  return theme;
}

bool SyntaxColorTheme::predefined() const {
  return m_predefined;
}

QString SyntaxColorTheme::name() const {
  return m_name;
}

QMap<SyntaxColorTheme::StyleComponents, SyntaxColorThemeComponent> SyntaxColorTheme::styleColors() const {
  return m_styleColors;
}

SyntaxColorThemeComponent::SyntaxColorThemeComponent(const QColor& fore, const QColor& back, bool bold, bool italic, bool underline)
  : m_colorForeground(fore), m_colorBackground(back), m_boldFont(bold), m_italicFont(italic), m_underlinedFont(underline) {}

void SyntaxColorThemeComponent::applyToEditor(TextEditor* editor, int style) {
  if (m_colorForeground.isValid()) {
    editor->styleSetFore(style, QCOLOR_TO_SPRT(m_colorForeground));
  }

  if (m_colorBackground.isValid()) {
    editor->styleSetBack(style, QCOLOR_TO_SPRT(m_colorBackground));
  }

  editor->styleSetBold(style, m_boldFont);
  editor->styleSetItalic(style, m_italicFont);
  editor->styleSetUnderline(style, m_underlinedFont);
}
