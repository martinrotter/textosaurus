// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef SETTINGSEXTERNALTOOLS_H
#define SETTINGSEXTERNALTOOLS_H

#include "gui/settings/settingspanel.h"

#include "ui_settingsexternaltools.h"

class SettingsExternalTools : public SettingsPanel {
  Q_OBJECT

  public:
    explicit SettingsExternalTools(Settings* settings, QWidget* parent = nullptr);

    inline QString title() const {
      return tr("External tools");
    }

    void loadSettings();

    void saveSettings();

  private:
    Ui::SettingsExternalTools m_ui;
};

#endif // SETTINGSEXTERNALTOOLS_H
