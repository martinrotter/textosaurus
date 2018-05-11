// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef MACROSSIDEBAR_H
#define MACROSSIDEBAR_H

#include "saurus/gui/sidebars/basesidebar.h"

#include "saurus/plugin-system/macros/macros.h"

class TextApplication;
class MacrosWidget;

class MacrosSidebar : public BaseSidebar {
  Q_OBJECT

  public:
    explicit MacrosSidebar(TextApplication* text_app, Macros* macros_factory, QWidget* parent = nullptr);
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
