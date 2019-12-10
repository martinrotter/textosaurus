// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "saurus/plugin-system/charactermap/charactermapwidget.h"

#include "common/miscellaneous/iofactory.h"
#include "definitions/definitions.h"
#include "saurus/plugin-system/charactermap/charactermap.h"

#include <QComboBox>
#include <QDebug>
#include <QFormLayout>
#include <QLineEdit>
#include <QScrollArea>
#include <QScrollBar>

CharacterMapWidget::CharacterMapWidget(QWidget* parent)
  : QWidget(parent), m_scrollForMap(nullptr), m_map(nullptr), m_cmbPlane(nullptr), m_allCharacters(QList<CharacterInfo>()) {
  setupUi();

  // Load characted categories.
  loadCategories();
  loadCharacters();

  updateVisibleCharacters();
}

void CharacterMapWidget::setupUi() {
  m_map = new CharacterMap(this);

  m_scrollForMap = new QScrollArea(this);

  m_scrollForMap->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOn);
  m_scrollForMap->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
  m_scrollForMap->setWidget(m_map);
  m_scrollForMap->setWidgetResizable(true);

  m_cmbPlane = new QComboBox(this);

  m_txtSearch = new QLineEdit(this);
  m_txtSearch->setPlaceholderText(tr("Search for symbol"));
  m_txtSearch->setClearButtonEnabled(true);

  QGridLayout* central_layout = new QGridLayout(this);

  central_layout->setMargin(0);
  central_layout->addWidget(m_cmbPlane);
  central_layout->addWidget(m_txtSearch);
  central_layout->addWidget(m_scrollForMap);

  connect(m_cmbPlane, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [=]() {
    updateVisibleCharacters();
  });
  connect(m_txtSearch, &QLineEdit::textChanged, this, [=]() {
    updateVisibleCharacters();
  });

  setLayout(central_layout);
}

void CharacterMapWidget::loadCategories() {
  m_cmbPlane->addItem(tr("All Symbols"), QVariant::fromValue(CharacterCategory(-1, -1)));

  QString blocks = QString::fromLocal8Bit(IOFactory::readFile(QSL(":/unicode/Blocks.txt")));
  QRegularExpression exp(QSL("^([\\dA-F]+)\\.\\.([\\dA-F]+); ?(.+)$"), QRegularExpression::PatternOption::MultilineOption);

  exp.optimize();

  QRegularExpressionMatchIterator i = exp.globalMatch(blocks);

  while (i.hasNext()) {
    QRegularExpressionMatch match = i.next();
    unsigned int to = match.captured(2).toInt(nullptr, 16);

    if (to <= 0xFFFF) {
      m_cmbPlane->addItem(match.captured(3), QVariant::fromValue(CharacterCategory(match.captured(1).toInt(nullptr, 16),
                                                                                   to)));
    }
  }
}

void CharacterMapWidget::loadCharacters() {
  QString chars = QString::fromLocal8Bit(IOFactory::readFile(QSL(":/unicode/UnicodeData.txt")));
  QRegularExpression exp(QSL("^([\\dA-F]+);([^;]+)"), QRegularExpression::PatternOption::MultilineOption);

  exp.optimize();

  QRegularExpressionMatchIterator i = exp.globalMatch(chars);

  while (i.hasNext()) {
    QRegularExpressionMatch match = i.next();

    if (match.captured(2) != QL1S("<control>")) {
      unsigned int cod = match.captured(1).toUInt(nullptr, 16);

      if (cod <= 0xFFFF) {
        m_allCharacters << CharacterInfo(QChar(cod), match.captured(2));
      }
    }
  }
}

CharacterMap* CharacterMapWidget::map() const {
  return m_map;
}

void CharacterMapWidget::updateVisibleCharacters() {
  if (!m_allCharacters.isEmpty()) {
    auto category = m_cmbPlane->currentData().value<CharacterCategory>();

    QList<CharacterInfo> categorised = charactersForCategory(category, m_txtSearch->text());

    m_scrollForMap->verticalScrollBar()->setValue(0);
    m_map->loadCharacters(categorised);
  }
}

QList<CharacterInfo> CharacterMapWidget::charactersForCategory(const CharacterCategory& cat, const QString& txt) const {
  if (cat.m_from < 0) {
    if (txt.isEmpty()) {
      return m_allCharacters;
    }
    else {
      QList<CharacterInfo> chars;

      for (int i = 0; i < m_allCharacters.size(); i++) {
        auto character_at = m_allCharacters.at(i);

        if (character_at.m_description.contains(txt, Qt::CaseSensitivity::CaseInsensitive)) {
          chars << character_at;
        }
      }

      return chars;
    }
  }
  else {
    QList<CharacterInfo> chars;

    for (int i = 0; i < m_allCharacters.size(); i++) {
      auto character_at = m_allCharacters.at(i);

      if ((character_at.m_character >= cat.m_from && character_at.m_character <= cat.m_to) &&
          (txt.isEmpty() || character_at.m_description.contains(txt, Qt::CaseSensitivity::CaseInsensitive))) {
        chars << character_at;
      }

      if (character_at.m_character > cat.m_to && txt.isEmpty()) {
        break;
      }
    }

    return chars;
  }
}
