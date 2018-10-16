// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifndef FORMFINDREPLACE_H
#define FORMFINDREPLACE_H

#include <QDialog>

#include "ui_formfindreplace.h"

namespace Ui {
  class FormFindReplace;
}

class TextApplication;

class FormFindReplace : public QDialog {
  Q_OBJECT

  public:
    explicit FormFindReplace(TextApplication* app, QWidget* parent = nullptr);
    virtual ~FormFindReplace() = default;

  public slots:
    void display();

  private slots:
    void clearTargetRange();
    void displayCount();
    void searchNext();
    void searchPrevious();
    void searchAll();
    void replaceAll();
    void searchReplaceNext();

  private:
    int extractFlags();

    void searchReplaceOne(bool reverse);
    void searchOne(bool reverse);

  private:
    Ui::FormFindReplace m_ui = {};
    TextApplication* m_application;
};

#endif // FORMFINDREPLACE_H
