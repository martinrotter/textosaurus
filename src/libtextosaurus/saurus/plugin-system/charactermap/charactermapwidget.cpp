// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "saurus/plugin-system/charactermap/charactermapwidget.h"

#include "definitions/definitions.h"
#include "saurus/plugin-system/charactermap/charactermap.h"

#include <QComboBox>
#include <QDebug>
#include <QFormLayout>
#include <QLineEdit>
#include <QScrollArea>

CharacterMapWidget::CharacterMapWidget(QWidget* parent) : QWidget(parent), m_map(nullptr), m_cmbPlane(nullptr) {
  setupUi();
}

void CharacterMapWidget::setupUi() {
  m_map = new CharacterMap(this);

  QScrollArea* scroll_map = new QScrollArea(this);

  scroll_map->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOn);
  scroll_map->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
  scroll_map->setWidget(m_map);
  scroll_map->setWidgetResizable(true);

  m_cmbPlane = new QComboBox(this);
  m_cmbPlane->addItem(tr("All Symbols"));

  m_txtSearch = new QLineEdit(this);
  m_txtSearch->setPlaceholderText(tr("Search for symbol"));
  m_txtSearch->setClearButtonEnabled(true);

  QGridLayout* central_layout = new QGridLayout(this);

  central_layout->setMargin(0);
  central_layout->addWidget(m_cmbPlane);
  central_layout->addWidget(m_txtSearch);
  central_layout->addWidget(scroll_map);

  setLayout(central_layout);

  m_map->loadCharacters({
    CharacterInfo(QChar(53), "DIGIT FIVE"),
    CharacterInfo(QChar(53), "DIGIT FIVE"),
    CharacterInfo(QChar(53), "DIGIT FIVE"),
    CharacterInfo(QChar(53), "DIGIT FIVE"),
    CharacterInfo(QChar(53), "DIGIT FIVE"),
    CharacterInfo(QChar(53), "DIGIT FIVE"),
    CharacterInfo(QChar(54), "DIGIT FOUR")
  });
}

CharacterMap* CharacterMapWidget::map() const {
  return m_map;
}
