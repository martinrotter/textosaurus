// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef SYNTAXHIGHLIGHTING_H
#define SYNTAXHIGHLIGHTING_H

#include <QObject>

#include <Qsci/qscilexer.h>

#include <functional>

#define LAMBDA_LEX_GEN(lex) []() { return new QsciLexer ## lex ## (); }

struct Lexer {
  explicit Lexer();
  explicit Lexer(const QString& name, const QStringList& suffices, const std::function<QsciLexer*()>& lexer_generator);

  bool isEmpty() const;

  QString m_name;
  QStringList m_suffices;

  std::function<QsciLexer*()> m_lexerGenerator;
};

typedef QList<Lexer> Lexers;

class SyntaxHighlighting : public QObject {
  Q_OBJECT

  public:
    explicit SyntaxHighlighting(QObject* parent = nullptr);

    QStringList fileFilters();
    Lexers lexers();

    // Returns lexer suitable for syntax highlighting of given file and filter.
    // NOTE: Caller takes ownership if the lexer.
    Lexer lexerForFile(const QString& file_name, const QString& file_filter);
    Lexer lexerForSuffix(const QString& suffix);

  private:
    QStringList m_fileFilters;
    Lexers m_lexers;
};

#endif // SYNTAXHIGHLIGHTING_H
