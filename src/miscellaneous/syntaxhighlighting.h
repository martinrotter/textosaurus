// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef SYNTAXHIGHLIGHTING_H
#define SYNTAXHIGHLIGHTING_H

#define SOLAR_LIGHT_BASE_3    253, 246, 227
#define SOLAR_LIGHT_BLUE      38, 139, 210
#define SOLAR_LIGHT_MAGENTA   211, 54, 130
#define SOLAR_LIGHT_ORANGE    203, 75, 22
#define SOLAR_LIGHT_BASE_00   101, 123, 131
#define SOLAR_LIGHT_VIOLET    108, 113, 196
#define SOLAR_LIGHT_BASE_1    147, 161, 161
#define SOLAR_LIGHT_BASE_2    238, 232, 213
#define SOLAR_LIGHT_CONTROL   198, 205, 205

#define SOLAR_BASE_3    0, 43, 54
#define SOLAR_BASE_00   131, 148, 150
#define SOLAR_BASE_1    147, 161, 161
#define SOLAR_BASE_2    7, 54, 66
#define SOLAR_CONTROL   78, 90, 90
#define SOLAR_GREEN     133, 153, 0
#define SOLAR_VIOLET    108, 113, 196
#define SOLAR_RED       220, 50, 47
#define SOLAR_ORANGE    203, 75, 22
#define SOLAR_BASE_01   88, 110, 117

#define MONOKAI_DARK    36, 36, 36        // paper
#define MONOKAI_DARK_L  89, 89, 89        // control chars
#define MONOKAI_LIGHT   249, 245, 221     // base font
#define MONOKAI_RED     255, 87, 148      // operators
#define MONOKAI_GREEN   178, 230, 76      // identifiers
#define MONOKAI_ORANGE  250, 162, 61      // secondary keywords
#define MONOKAI_PURPLE  170, 125, 252     // numbers
#define MONOKAI_BROWN   204, 141, 102     // comments
#define MONOKAI_YELLOW  230, 219, 122     // string literals
#define MONOKAI_CYAN    102, 204, 225     // keywords

#include <QObject>

#include "miscellaneous/syntaxcolortheme.h"

#include <QColor>
#include <QMap>

struct Lexer {
  explicit Lexer();
  explicit Lexer(const QString& name, const QStringList& suffices, int code,
                 const QMap<int, SyntaxColorTheme::StyleComponents>& style_mappings = QMap<int, SyntaxColorTheme::StyleComponents>(),
                 const QString& keywords = QString());
  virtual ~Lexer() = default;

  bool isEmpty() const;

  QString m_name;
  int m_code;
  QStringList m_suffices;
  QString m_keywords;

  QMap<int, SyntaxColorTheme::StyleComponents> m_styleMappings;
};

Q_DECLARE_METATYPE(Lexer)

class SyntaxHighlighting : public QObject {
  Q_OBJECT

  public:
    explicit SyntaxHighlighting(QObject* parent = nullptr);
    virtual ~SyntaxHighlighting() = default;

    QStringList bareFileFilters();
    QStringList fileFilters();
    QList<Lexer> lexers();
    Lexer defaultLexer();
    QList<SyntaxColorTheme> colorThemes();
    SyntaxColorTheme currentColorTheme();

    void setCurrentColorTheme(const QString& theme_name);
    void saveColorThemes(const QList<SyntaxColorTheme>& themes, int curr_theme_index);

    // Returns lexer suitable for syntax highlighting of given file and filter.
    // NOTE: Caller takes ownership of the lexer.
    Lexer lexerForFile(const QString& file_name);
    Lexer lexerForSuffix(const QString& suffix);
    Lexer lexerForName(const QString& name);

  private:
    void loadColorThemes();

  private:
    QStringList m_bareFileFilters;
    QStringList m_fileFilters;

    QList<Lexer> m_lexers;
    QList<SyntaxColorTheme> m_colorThemes;
    int m_currentColorThemeIndex;
};

#endif // SYNTAXHIGHLIGHTING_H
