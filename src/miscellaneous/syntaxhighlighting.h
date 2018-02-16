// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef SYNTAXHIGHLIGHTING_H
#define SYNTAXHIGHLIGHTING_H

#include <QObject>

#include "miscellaneous/syntaxcolortheme.h"

#include <QColor>
#include <QMap>

struct Lexer {
  explicit Lexer();
  explicit Lexer(const QString& name, const QStringList& suffices, int code,
                 const QMap<int, SyntaxColorTheme::StyleComponents>& style_mappings = QMap<int, SyntaxColorTheme::StyleComponents>());
  virtual ~Lexer() = default;

  bool isEmpty() const;

  QString m_name;
  int m_code;
  QStringList m_suffices;

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
    SyntaxColorTheme defaultTheme();

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
};

#endif // SYNTAXHIGHLIGHTING_H
