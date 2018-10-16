// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifndef SETTINGSGUI_H
#define SETTINGSGUI_H

#include "saurus/gui/settings/settingspanel.h"

#include "ui_settingsgui.h"

class SettingsGui : public SettingsPanel {
  Q_OBJECT

  public:
    explicit SettingsGui(Settings* settings, QWidget* parent = nullptr);
    virtual ~SettingsGui() = default;

    virtual QString title() const override;
    virtual void loadSettings() override;
    virtual void saveSettings() override;

  protected:
    virtual bool eventFilter(QObject* obj, QEvent* e) override;

  private:
    Ui::SettingsGui m_ui = {};
};

inline QString SettingsGui::title() const {
  return tr("User Interface");
}

#endif // SETTINGSGUI_H
