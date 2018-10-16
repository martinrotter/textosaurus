// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifndef FORMSETTINGS_H
#define FORMSETTINGS_H

#include <QDialog>

#include "ui_formsettings.h"

class Settings;
class SettingsPanel;

class FormSettings : public QDialog {
  Q_OBJECT

  public:
    explicit FormSettings(QWidget& parent);
    virtual ~FormSettings() = default;

  private slots:

    // Saves settings into global configuration.
    void saveSettings();
    void applySettings();
    void cancelSettings();

  private:
    void addSettingsPanel(SettingsPanel* panel);

    Ui::FormSettings m_ui = {};
    QPushButton* m_btnApply;

    QList<SettingsPanel*> m_panels;
    Settings& m_settings;
};

#endif // FORMSETTINGS_H
