// For license of this file, see <project-root-folder>/LICENSE.md.

#include "saurus/miscellaneous/macros.h"

Macros::Macros(QObject* parent) : QObject(parent) {}

Macros::~Macros() {
  clearAllMacros();
}

void Macros::addMacro(Macro* macro) {
  macro->setParent(this);
  m_storedMacros.append(macro);
}

void Macros::clearAllMacros() {
  qDeleteAll(m_storedMacros);
  m_storedMacros.clear();
}

void Macros::loadMacrosFromSettings(const Settings* settings) {
  clearAllMacros();

  // TODO: dodělat
}
