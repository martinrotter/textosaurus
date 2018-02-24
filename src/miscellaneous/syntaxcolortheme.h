// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef SYNTAXCOLORTHEME_H
#define SYNTAXCOLORTHEME_H

#include <QObject>

#include <QColor>
#include <QMap>

class TextEditor;
class QSettings;

struct SyntaxColorThemeComponent {
  explicit SyntaxColorThemeComponent() = default;
  explicit SyntaxColorThemeComponent(const QColor& fore, const QColor& back = QColor(), bool bold = false,
                                     bool italic = false, bool underline = false);

  void applyToEditor(TextEditor& editor, int style);

  QColor m_colorForeground;
  QColor m_colorBackground;
  bool m_boldFont;
  bool m_italicFont;
  bool m_underlinedFont;
};

class SyntaxColorTheme : public QObject {
  Q_OBJECT

  public:
    enum class StyleComponents {
      // Default text.
      Default = 0,

      // All kinds of comments.
      Comment = 1,

      // Numbers.
      Number = 2,

      // String or character literals.
      String = 3,

      // Operators.
      Operator = 4,

      // Identifiers - names of variables/attributes etc.
      Identifier = 5,

      // Keywords.
      Keyword = 6,

      // Conditionally compiled pieces of code, preprocessor directives etc.
      Preprocessor = 7,

      // Regular expression strings.
      Regex = 8,

      // Blocks of foreign code.
      Script = 11,

      // Verbatim plain data in code, for example CDATA in XML.
      PlainData = 12,

      // Errorneous parts of code.
      Error = 13,

      //ScintillaDefault = 1000,
      ScintillaMargin = 1001,

      //ScintillaBraceLight = 1002,
      //ScintillaBraceBad = 1003,
      ScintillaControlChar = 1004,
      ScintillaIndentGuide = 1005,

      //ScintillaCallTip = 1006,
      //ScintillaFoldDisplayText = 1007,
      ScintillaPaper = 1008
    };

    Q_ENUM(StyleComponents)

    explicit SyntaxColorTheme(const QString& name, bool predefined,
                              const QMap<StyleComponents,
                                         SyntaxColorThemeComponent>& styles = QMap<StyleComponents, SyntaxColorThemeComponent>());
    SyntaxColorTheme(const SyntaxColorTheme& another);
    SyntaxColorTheme(SyntaxColorTheme&& another);
    virtual ~SyntaxColorTheme() = default;

    SyntaxColorTheme& operator=(const SyntaxColorTheme& other);

    bool predefined() const;
    bool hasComponent(StyleComponents code) const;

    SyntaxColorThemeComponent component(StyleComponents code);
    SyntaxColorThemeComponent randomizedComponent(int scintilla_code);
    QString name() const;
    QMap<StyleComponents, SyntaxColorThemeComponent> styleColors() const;
    void clearStyleColors();

    void removeComponent(StyleComponents code);

    void setName(const QString& name);
    void setPredefined(bool predefined);
    void setComponent(StyleComponents code, const SyntaxColorThemeComponent& compon);

    void toSettings(QSettings& settings) const;

    static QList<SyntaxColorTheme> fromSettings(QSettings& settings);

  private:
    QMap<StyleComponents, SyntaxColorThemeComponent> m_styleColors;
    QString m_name;
    bool m_predefined;
};

#endif // SYNTAXCOLORTHEME_H
