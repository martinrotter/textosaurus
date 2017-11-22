// For license of this file, see <project-root-folder>/LICENSE.md.

#include "gui/dialogs/formfindreplace.h"

FormFindReplace::FormFindReplace(QWidget* parent) : QDialog(parent) {
  m_ui.setupUi(this);

  setWindowFlags(Qt::Tool | Qt::WindowTitleHint | Qt::WindowStaysOnTopHint | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
  setWindowOpacity(0.8);
}

FormFindReplace::~FormFindReplace() {}
