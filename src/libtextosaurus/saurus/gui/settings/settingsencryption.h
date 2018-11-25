#ifndef SETTINGSENCRYPTION_H
#define SETTINGSENCRYPTION_H

// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "saurus/gui/settings/settingspanel.h"

#include "ui_settingsencryption.h"

class SettingsEncryption : public SettingsPanel {
  Q_OBJECT

  public:
    explicit SettingsEncryption(Settings* settings, QWidget* parent = nullptr);
    virtual ~SettingsEncryption() = default;

    virtual QString title() const override;
    virtual void loadSettings() override;
    virtual void saveSettings() override;

  private:
    Ui::SettingsEncryption ui = {};
};

inline QString SettingsEncryption::title() const {
  return tr("Encryption");
}

#endif // SETTINGSENCRYPTION_H
