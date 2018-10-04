// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifndef FORMDECRYPTPASSWORDPROMPT_H
#define FORMDECRYPTPASSWORDPROMPT_H

#include <QDialog>

#include "ui_formdecryptpasswordprompt.h"

#include <QFile>

namespace Ui {
  class FormDecryptPasswordPrompt;
}

class FormDecryptPasswordPrompt : public QDialog {
  Q_OBJECT

  public:
    explicit FormDecryptPasswordPrompt(QFile& file, QWidget* parent = nullptr);
    virtual ~FormDecryptPasswordPrompt();

    QString password() const;

    static QString getPasswordFromUser(QFile& file, bool& ok);

  private:
    Ui::FormDecryptPasswordPrompt m_ui;
};

#endif // FORMDECRYPTPASSWORDPROMPT_H
