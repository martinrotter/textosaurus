// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef LANGUAGESELECTOR_H
#define LANGUAGESELECTOR_H

#include <QWidget>

#include "miscellaneous/syntaxhighlighting.h"

class QVBoxLayout;
class QLineEdit;
class QRadioButton;
class QScrollArea;

class LanguageSelector : public QWidget {
  Q_OBJECT

  public:
    explicit LanguageSelector(const Lexers& lexers, QWidget* parent = nullptr);

    void setActiveLexer(const Lexer& lexer);
    void clearActiveLexer();

  signals:
    void languageChanged(Lexer lexer);

  protected:
    void showEvent(QShowEvent* event);

  private slots:
    void filterLexersMenu();
    void lexerActivated();

  private:
    void loadLexers(const Lexers& lexers);

  private:
    QScrollArea* m_scrollArea;
    QLineEdit* m_txtLexerFilter;

    QList<QRadioButton*> m_rbButtons;
};

#endif // LANGUAGESELECTOR_H
