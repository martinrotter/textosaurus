// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef SYNTAXHIGHLIGHTING_H
#define SYNTAXHIGHLIGHTING_H

#include <QObject>

#include <Qsci/qscilexer.h>

#include <functional>

#define LAMBDA_LEX_GEN(lex) []() { return new QsciLexer##lex##(); }

struct Lexer {
  explicit Lexer(const QString& name, const QStringList& suffices, const std::function<QsciLexer*()>& lexer_generator);

  QString m_name;
  QStringList m_suffices;

  std::function<QsciLexer*()> m_lexerGenerator;
};

typedef QList<Lexer> LexerSuffices;

class SyntaxHighlighting : public QObject {
  Q_OBJECT

  public:
    explicit SyntaxHighlighting(QObject* parent = nullptr);

    QStringList fileFilters();
    LexerSuffices lexers();

    // Returns lexer suitable for syntax highlighting of given file and filter.
    // NOTE: Caller takes ownership if the lexer.
    QsciLexer* lexerForFile(const QString& file_name, const QString& file_filter);
    QsciLexer* lexerForSuffix(const QString& suffix);

  private:
    QStringList m_fileFilters;
    LexerSuffices m_lexers;
};

#endif // SYNTAXHIGHLIGHTING_H
