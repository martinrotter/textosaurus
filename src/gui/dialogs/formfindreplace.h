// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef FORMFINDREPLACE_H
#define FORMFINDREPLACE_H

#include <QDialog>

#include "ui_formfindreplace.h"

namespace Ui {
  class FormFindReplace;
}

class FormFindReplace : public QDialog {
  Q_OBJECT

  public:
    explicit FormFindReplace(QWidget* parent = nullptr);
    virtual ~FormFindReplace();

  private:
    Ui::FormFindReplace m_ui;
};

#endif // FORMFINDREPLACE_H
