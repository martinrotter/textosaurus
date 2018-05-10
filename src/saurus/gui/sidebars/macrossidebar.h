// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef MACROSSIDEBAR_H
#define MACROSSIDEBAR_H

#include "saurus/gui/sidebars/basesidebar.h"

#include "saurus/miscellaneous/macros.h"

class TextApplication;
class MacrosWidget;

class MacrosSidebar : public BaseSidebar {
  Q_OBJECT

  public:
    explicit MacrosSidebar(TextApplication* app, QWidget* parent = nullptr);
    virtual ~MacrosSidebar();

    Qt::DockWidgetArea initialArea() const;
    bool initiallyVisible() const;
    int initialWidth() const;

  public slots:
    void load();
    void startRecording();
    void stopRecording();
    void saveMacroAs();
    void playMacro();

  private slots:
    void loadNewRecordedMacroStep(Macro::MacroStep step);

  private:
    MacrosWidget* m_widget = nullptr;
    QAction* m_actionRecordStart;
    QAction* m_actionRecordStop;
    QAction* m_actionPlay;
    QAction* m_actionSave;
};

#endif // MACROSSIDEBAR_H
