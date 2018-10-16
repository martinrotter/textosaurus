// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifndef SETTINGSGENERAL_H
#define SETTINGSGENERAL_H

#include "saurus/gui/settings/settingspanel.h"

#include "ui_settingsgeneral.h"

class SettingsGeneral : public SettingsPanel {
  Q_OBJECT

  public:
    explicit SettingsGeneral(Settings* settings, QWidget* parent = nullptr);
    virtual ~SettingsGeneral() = default;

    virtual QString title() const override;
    virtual void loadSettings() override;
    virtual void saveSettings() override;

  private:
    Ui::SettingsGeneral m_ui = {};
};

inline QString SettingsGeneral::title() const {
  return tr("General");
}

#endif // SETTINGSGENERAL_H
