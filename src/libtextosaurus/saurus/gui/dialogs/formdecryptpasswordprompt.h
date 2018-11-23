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
    explicit FormDecryptPasswordPrompt(const QByteArray& data, QWidget* parent = nullptr);
    virtual ~FormDecryptPasswordPrompt() = default;

    QString password() const;

    static QString getPasswordFromUser(const QByteArray& data, bool& ok);

  private:
    Ui::FormDecryptPasswordPrompt m_ui = {};
};

#endif // FORMDECRYPTPASSWORDPROMPT_H
