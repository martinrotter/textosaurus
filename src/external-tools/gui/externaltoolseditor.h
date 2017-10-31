// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef EXTERNALTOOLSEDITOR_H
#define EXTERNALTOOLSEDITOR_H

#include <QDialog>

#include "ui_externaltoolseditor.h"

namespace Ui {
  class ExternalToolsEditor;
}

class ExternalToolsEditor : public QDialog {
  Q_OBJECT

  public:
    explicit ExternalToolsEditor(QWidget* parent = nullptr);

  private:
    Ui::ExternalToolsEditor m_ui;
};

#endif // EXTERNALTOOLSEDITOR_H
