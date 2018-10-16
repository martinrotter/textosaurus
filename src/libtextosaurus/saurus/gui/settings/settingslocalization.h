// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifndef SETTINGSLOCALIZATION_H
#define SETTINGSLOCALIZATION_H

#include "saurus/gui/settings/settingspanel.h"

#include "ui_settingslocalization.h"

class SettingsLocalization : public SettingsPanel {
  Q_OBJECT

  public:
    explicit SettingsLocalization(Settings* settings, QWidget* parent = nullptr);
    virtual ~SettingsLocalization() = default;

    virtual QString title() const override;
    virtual void loadSettings() override;
    virtual void saveSettings() override;

  private:
    Ui::SettingsLocalization m_ui = {};
};

inline QString SettingsLocalization::title() const {
  return tr("Localization");
}

#endif // SETTINGSLOCALIZATION_H
