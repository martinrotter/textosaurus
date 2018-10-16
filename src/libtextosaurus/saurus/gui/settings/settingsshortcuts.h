// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifndef SETTINGSSHORTCUTS_H
#define SETTINGSSHORTCUTS_H

#include "saurus/gui/settings/settingspanel.h"

#include "ui_settingsshortcuts.h"

class SettingsShortcuts : public SettingsPanel {
  Q_OBJECT

  public:
    explicit SettingsShortcuts(Settings* settings, QWidget* parent = nullptr);
    virtual ~SettingsShortcuts() = default;

    virtual QString title() const override;
    virtual void loadSettings() override;
    virtual void saveSettings() override;

  private:
    Ui::SettingsShortcuts m_ui = {};
};

inline QString SettingsShortcuts::title() const {
  return tr("Keyboard Shortcuts");
}

#endif // SETTINGSSHORTCUTS_H
