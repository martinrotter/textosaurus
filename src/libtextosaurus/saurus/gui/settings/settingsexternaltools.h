// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifndef SETTINGSEXTERNALTOOLS_H
#define SETTINGSEXTERNALTOOLS_H

#include "saurus/gui/settings/settingspanel.h"

#include "ui_settingsexternaltools.h"

class SettingsExternalTools : public SettingsPanel {
  Q_OBJECT

  public:
    explicit SettingsExternalTools(Settings* settings, QWidget* parent = nullptr);
    virtual ~SettingsExternalTools() = default;

    virtual QString title() const override;
    virtual void loadSettings() override;
    virtual void saveSettings() override;

  protected slots:
    virtual void dirtifySettings() override;

  private slots:
    void addNewTool();
    void removeSelectedTool();
    void saveCurrentTool();
    void updateToolListNames(const QString& name);
    void saveToolChanges(QListWidgetItem* item);
    void displayToolDetails(QListWidgetItem* current, QListWidgetItem* previous);

  private:
    Ui::SettingsExternalTools m_ui = {};
    bool m_isSwitchingSelectedTool;
};

inline QString SettingsExternalTools::title() const {
  return tr("External Tools");
}

#endif // SETTINGSEXTERNALTOOLS_H
