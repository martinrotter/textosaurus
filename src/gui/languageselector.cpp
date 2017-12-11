// For license of this file, see <project-root-folder>/LICENSE.md.

#include "gui/languageselector.h"

#include "definitions/definitions.h"

#include <QLineEdit>
#include <QRadioButton>
#include <QScrollArea>
#include <QScrollBar>
#include <QVariant>
#include <QVBoxLayout>

LanguageSelector::LanguageSelector(const Lexers& lexers, QWidget* parent) : QWidget(parent), m_rbButtons(QList<QRadioButton*>()) {
  loadLexers(lexers);
}

void LanguageSelector::setActiveLexer(const Lexer& lexer) {
  m_txtLexerFilter->clear();

  foreach (QRadioButton* btn, m_rbButtons) {
    if (btn->text().contains(lexer.m_name, Qt::CaseSensitivity::CaseInsensitive)) {
      btn->setChecked(true);
      m_scrollArea->verticalScrollBar()->setValue(btn->pos().y() - btn->height() * 3);
      return;
    }
  }
}

void LanguageSelector::clearActiveLexer() {
  m_txtLexerFilter->clear();
  m_rbButtons.first()->setChecked(true);
  m_scrollArea->verticalScrollBar()->setValue(0);
}

void LanguageSelector::showEvent(QShowEvent* event) {
  QWidget::showEvent(event);
  m_txtLexerFilter->setFocus();
}

void LanguageSelector::filterLexersMenu() {
  QString filter = m_txtLexerFilter->text();

  foreach (QRadioButton* btn, m_rbButtons) {
    btn->setVisible(filter.isEmpty() || btn->text().contains(filter, Qt::CaseSensitivity::CaseInsensitive));
  }
}

void LanguageSelector::lexerActivated() {
  QRadioButton* btn_sender = qobject_cast<QRadioButton*>(sender());
  emit languageChanged(btn_sender->property("lexer").value<Lexer>());
}

void LanguageSelector::loadLexers(const Lexers& lexers) {
  // Filtering textbox.
  m_txtLexerFilter = new QLineEdit(this);
  m_txtLexerFilter->setPlaceholderText(tr("Filter highlighters"));
  connect(m_txtLexerFilter, &QLineEdit::textChanged, this, &LanguageSelector::filterLexersMenu);

  // Initialize scroll area and layouts.
  QWidget* widget = new QWidget(this);
  QVBoxLayout* layout_rb = new QVBoxLayout(this);
  QVBoxLayout* layout_this = new QVBoxLayout(this);

  m_scrollArea = new QScrollArea(this);
  m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  m_scrollArea->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContentsOnFirstShow);

  foreach (const Lexer& lex, lexers) {
    QString lexer_name = QL1S("&") + lex.m_name;
    QRadioButton* rb_lexer = new QRadioButton(lexer_name, this);

    rb_lexer->setProperty("lexer", QVariant::fromValue<Lexer>(lex));
    layout_rb->addWidget(rb_lexer);
    m_rbButtons.append(rb_lexer);

    connect(rb_lexer, &QRadioButton::toggled, this, &LanguageSelector::lexerActivated);
  }

  layout_rb->addStretch();

  QMargins margins = layout_rb->contentsMargins();

  margins.setRight(margins.right() + 10);
  layout_rb->setContentsMargins(margins);
  widget->setLayout(layout_rb);

  layout_this->addWidget(m_txtLexerFilter);
  layout_this->addWidget(m_scrollArea);
  m_scrollArea->setWidget(widget);

  setLayout(layout_this);
}
