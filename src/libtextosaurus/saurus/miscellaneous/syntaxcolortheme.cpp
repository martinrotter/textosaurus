// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "saurus/miscellaneous/syntaxcolortheme.h"

#include "definitions/definitions.h"
#include "saurus/gui/texteditor.h"

#include <QList>
#include <QMetaEnum>
#include <QSettings>

#include <utility>

SyntaxColorTheme::SyntaxColorTheme(const QString& name, bool predefined, const QMap<StyleComponents, SyntaxColorThemeComponent>& styles)
  : m_styleColors(styles), m_name(name), m_predefined(predefined) {}

SyntaxColorTheme::SyntaxColorTheme(const SyntaxColorTheme& another) {
  *this = another;
}

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

void SyntaxColorTheme::removeComponent(SyntaxColorTheme::StyleComponents code) {
  m_styleColors.remove(code);
}

void SyntaxColorTheme::toSettings(QSettings& settings) const {
  QMetaEnum enum_converter = QMetaEnum::fromType<StyleComponents>();

  QMapIterator<StyleComponents, SyntaxColorThemeComponent> i(m_styleColors);

  settings.beginGroup(name());
  settings.remove(QString());

  while (i.hasNext()) {
    i.next();

    auto component = enum_converter.valueToKey(int(i.key()));
    auto style = i.value();

    settings.setValue(QString("component_%1").arg(component),
                      QString("%1;%2;%3;%4;%5").arg(style.m_colorForeground.isValid() ? style.m_colorForeground.name() : QString(),
                                                    style.m_colorBackground.isValid() ? style.m_colorBackground.name() : QString(),
                                                    QString::number(int(style.m_boldFont)),
                                                    QString::number(int(style.m_italicFont)),
                                                    QString::number(int(style.m_underlinedFont))));
  }

  settings.endGroup();
  settings.sync();
}

SyntaxColorTheme& SyntaxColorTheme::operator=(const SyntaxColorTheme& other) {
  if(&other == this) {
    return *this;
  }
  else {
    m_styleColors = other.m_styleColors;
    m_name = other.m_name;
    m_predefined = other.predefined();

    return *this;
  }
}

bool SyntaxColorTheme::hasComponent(SyntaxColorTheme::StyleComponents code) const {
  return m_styleColors.contains(code);
}

QList<SyntaxColorTheme> SyntaxColorTheme::fromSettings(QSettings& settings) {
  QStringList theme_names = settings.childGroups();

  QList<SyntaxColorTheme> themes;

  QMetaEnum enum_converter = QMetaEnum::fromType<SyntaxColorTheme::StyleComponents>();

  for (const QString& group : theme_names) {
    settings.beginGroup(group);

    SyntaxColorTheme theme(group, false);

    for (const QString& key : settings.childKeys()) {
      const QStringList raw_data = settings.value(key).toString().split(';');

      if (raw_data.size() == 5) {
        auto component = static_cast<SyntaxColorTheme::StyleComponents>(enum_converter.keyToValue(qPrintable(key.split('_').last())));
        SyntaxColorThemeComponent theme_component(raw_data.at(0),
                                                  raw_data.at(1),
                                                  raw_data.at(2).toInt(),
                                                  raw_data.at(3).toInt(),
                                                  raw_data.at(4).toInt());

        theme.setComponent(component, theme_component);
      }
      else {
        qDebug("Failed to parse key \"%s\" when loading color scheme \"%s\".", qPrintable(key), qPrintable(group));
      }
    }

    themes.append(theme);
    settings.endGroup();
  }

  return themes;
}

void SyntaxColorTheme::setPredefined(bool predefined) {
  m_predefined = predefined;
}

void SyntaxColorTheme::setComponent(SyntaxColorTheme::StyleComponents code, const SyntaxColorThemeComponent& compon) {
  m_styleColors[code] = compon;
}

void SyntaxColorTheme::setName(const QString& name) {
  m_name = name;
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

void SyntaxColorThemeComponent::applyToEditor(TextEditor& editor, int style) {
  if (m_colorForeground.isValid()) {
    editor.styleSetFore(style, QCOLOR_TO_SPRT(m_colorForeground));
  }

  if (m_colorBackground.isValid()) {
    editor.styleSetBack(style, QCOLOR_TO_SPRT(m_colorBackground));
  }

  editor.styleSetBold(style, m_boldFont);
  editor.styleSetItalic(style, m_italicFont);
  editor.styleSetUnderline(style, m_underlinedFont);
}
