// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef SETTINGSEDITOR_H
#define SETTINGSEDITOR_H

#include "gui/settings/settingspanel.h"

#include "ui_settingseditor.h"

class SettingsEditor : public SettingsPanel {
  Q_OBJECT

  public:
    explicit SettingsEditor(Settings* settings, QWidget* parent = nullptr);

    QString title() const;
    void loadSettings();
    void saveSettings();

  private slots:
    void changeMainFont();

  private:
    Ui::SettingsEditor m_ui;
};

inline QString SettingsEditor::title() const {
  return tr("Editor");
}

#endif // SETTINGSEDITOR_H