// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef FORMMAIN_H
#define FORMMAIN_H

#include <QMainWindow>

#include "ui_formmain.h"

class StatusBar;
class ToolBar;

class FormMain : public QMainWindow {
  Q_OBJECT

  friend class TabWidget;
  friend class TextApplication;

  public:

    // Constructors and destructors.
    explicit FormMain(QWidget* parent = nullptr);
    virtual ~FormMain();

    TabWidget* tabWidget() const;
    ToolBar* toolBar() const;
    StatusBar* statusBar() const;

    // Returns list of all globally available actions.
    // NOTE: This is used for setting dynamic shortcuts
    // for given actions.
    QList<QAction*> allActions() const;

    // Loads/saves visual state of the application.
    void loadSize();
    void saveSize();

  public slots:

    // Displays window on top or switches its visibility.
    void display();

    // Switches visibility of main window.
    void switchVisibility();

    // Turns on/off fullscreen mode
    void switchFullscreenMode();

  protected:
    void dragEnterEvent(QDragEnterEvent* event);
    void closeEvent(QCloseEvent* event);

  signals:
    void closeRequested(bool* should_stop);

  private slots:
    void showWiki();
    void reportABug();
    void donate();

  private:
    void prepareMenus();
    void createConnections();
    void setupIcons();

    Ui::FormMain m_ui;
    StatusBar* m_statusBar;
    ToolBar* m_toolBar;
};

#endif // FORMMAIN_H
