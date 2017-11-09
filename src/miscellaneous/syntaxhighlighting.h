// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef SYNTAXHIGHLIGHTING_H
#define SYNTAXHIGHLIGHTING_H

#include <QObject>

#include <functional>

#define LAMBDA_LEX_GEN(lex) []() { return new QsciLexer ## lex(); }

struct Lexer {
  explicit Lexer();
  explicit Lexer(const QString& name, const QStringList& suffices, int code);

  bool isEmpty() const;

  QString m_name;
  int m_code;
  QStringList m_suffices;
};

Q_DECLARE_METATYPE(Lexer)

typedef QList<Lexer> Lexers;

class SyntaxHighlighting : public QObject {
  Q_OBJECT

  public:
    explicit SyntaxHighlighting(QObject* parent = nullptr);

    QStringList fileFilters();
    Lexers lexers();

    Lexer defaultLexer();

    // Returns lexer suitable for syntax highlighting of given file and filter.
    // NOTE: Caller takes ownership if the lexer.
    Lexer lexerForFile(const QString& file_name);
    Lexer lexerForSuffix(const QString& suffix);
    Lexer lexerForName(const QString& name);

  private:
    QStringList m_fileFilters;
    Lexers m_lexers;
};

#endif // SYNTAXHIGHLIGHTING_H
