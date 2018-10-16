// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifndef FORMENCRYPTIONSETTINGS_H
#define FORMENCRYPTIONSETTINGS_H

#include <QDialog>

#include "ui_formencryptionsettings.h"

namespace Ui {
  class FormEncryptionSettings;
}

class FormEncryptionSettings : public QDialog {
  Q_OBJECT

  public:
    explicit FormEncryptionSettings(const QString& password, QWidget* parent = nullptr);
    virtual ~FormEncryptionSettings() = default;

    QString encryptionPassword() const;

  private slots:
    void updateResults();

  private:
    Ui::FormEncryptionSettings m_ui = {};
};

#endif // FORMENCRYPTIONSETTINGS_H
