// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef SETTINGSEXTERNALTOOLS_H
#define SETTINGSEXTERNALTOOLS_H

#include "gui/settings/settingspanel.h"

#include "ui_settingsexternaltools.h"

class SettingsExternalTools : public SettingsPanel {
  Q_OBJECT

  public:
    explicit SettingsExternalTools(Settings* settings, QWidget* parent = nullptr);

    QString title() const;
    void loadSettings();
    void saveSettings();

  private slots:
    void addNewTool();
    void removeSelectedTool();
    void saveCurrentTool();
    void updateToolListNames(const QString& name);
    void saveToolChanges(QListWidgetItem* item);
    void displayToolDetails(QListWidgetItem* current, QListWidgetItem* previous);

  private:
    Ui::SettingsExternalTools m_ui;
};

inline QString SettingsExternalTools::title() const {
  return tr("External tools");
}

#endif // SETTINGSEXTERNALTOOLS_H
