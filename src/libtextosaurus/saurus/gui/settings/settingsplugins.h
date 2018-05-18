// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef SETTINGSPLUGINS_H
#define SETTINGSPLUGINS_H

#include "saurus/gui/settings/settingspanel.h"

#include "ui_settingsplugins.h"

class SettingsPlugins : public SettingsPanel {
  Q_OBJECT

  public:
    explicit SettingsPlugins(Settings* settings, QWidget* parent = nullptr);
    virtual ~SettingsPlugins() = default;

    virtual QString title() const override;
    virtual void loadSettings() override;
    virtual void saveSettings() override;

  private:
    Ui::SettingsPlugins m_ui;
};

#endif // SETTINGSPLUGINS_H