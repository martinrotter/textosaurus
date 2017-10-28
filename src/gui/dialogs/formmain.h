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
    explicit FormMain(QWidget* parent = nullptr, Qt::WindowFlags f = 0);
    virtual ~FormMain();

    // Returns global tab widget.
    TabWidget* tabWidget() const;
    ToolBar* toolBar() const;

    // Access to statusbar.
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

  private slots:
    void ensureToolBarVisibility();

    // Displays various dialogs.
    void showWiki();
    void reportABug();
    void donate();

  private:

    // Creates all needed menus and sets them up.
    void prepareMenus();

    // Creates needed connections for this window.
    void createConnections();

    // Sets up proper icons for this widget.
    void setupIcons();

    QScopedPointer<Ui::FormMain> m_ui;
    StatusBar* m_statusBar;
    ToolBar* m_toolBar;
};

#endif // FORMMAIN_H
