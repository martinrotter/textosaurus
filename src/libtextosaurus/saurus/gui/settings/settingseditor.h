// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifndef SETTINGSEDITOR_H
#define SETTINGSEDITOR_H

#include "saurus/gui/settings/settingspanel.h"

#include "ui_settingseditor.h"

class SettingsEditor : public SettingsPanel {
  Q_OBJECT

  public:
    explicit SettingsEditor(Settings* settings, QWidget* parent = nullptr);
    virtual ~SettingsEditor() = default;

    virtual QString title() const override;
    virtual void loadSettings() override;
    virtual void saveSettings() override;

  private slots:
    void changeMainFont();

  private:
    Ui::SettingsEditor m_ui = {};
};

inline QString SettingsEditor::title() const {
  return tr("Editor");
}

#endif // SETTINGSEDITOR_H
