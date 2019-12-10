// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifndef MACROSSIDEBAR_H
#define MACROSSIDEBAR_H

#include "saurus/gui/sidebars/basesidebar.h"

#include "saurus/plugin-system/macros/macros.h"

class MacrosWidget;
class MacrosPlugin;

class MacrosSidebar : public BaseSidebar {
  Q_OBJECT

  public:
    explicit MacrosSidebar(MacrosPlugin* plugin, Macros* macros_factory, QWidget* parent = nullptr);
    virtual ~MacrosSidebar() = default;

    virtual Qt::DockWidgetArea initialArea() const override;
    virtual bool initiallyVisible() const override;
    virtual int initialWidth() const override;

  public slots:
    void startRecording();
    void stopRecording();
    void saveRecordedMacroAs();
    void playRecordedMacro();
    void playStoredMacro();
    void deleteStoredMacro();

  private slots:
    void onCurrentStoredMacroChanged(int row);
    void loadNewRecordedMacroStep(Macro::MacroStep step);

  protected:
    virtual void load() override;

  private:
    void reloadStoredMacros();

  private:
    MacrosPlugin* m_plugin;
    Macros* m_macrosFactory;
    MacrosWidget* m_widget = nullptr;
    QAction* m_actionRecordStart;
    QAction* m_actionRecordStop;
    QAction* m_actionRecordPlay;
    QAction* m_actionRecordSave;
    QAction* m_actionStoredPlay;
    QAction* m_actionStoredDelete;
};

#endif // MACROSSIDEBAR_H
