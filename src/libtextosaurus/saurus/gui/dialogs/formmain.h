// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifndef FORMMAIN_H
#define FORMMAIN_H

#include <QMainWindow>

#include "definitions/definitions.h"

#include "ui_formmain.h"

class StatusBar;
class ToolBar;
class TabWidget;

class TEXTOSAURUS_DLLSPEC FormMain : public QMainWindow {
  Q_OBJECT

  friend class TabWidget;
  friend class Application;
  friend class TextApplication;

  public:
    explicit FormMain(QWidget* parent = nullptr);
    virtual ~FormMain() = default;

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
    void switchVisibility(bool force_hide = false);

    void switchStayOnTop();

    // Turns on/off fullscreen mode
    void switchFullscreenMode();

  protected:
    virtual void changeEvent(QEvent* event) override;
    virtual void dragEnterEvent(QDragEnterEvent* event) override;
    virtual void closeEvent(QCloseEvent* event) override;

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

    Ui::FormMain m_ui = {};
    TabWidget* m_tabEditors;
    StatusBar* m_statusBar;
    ToolBar* m_toolBar;
};

#endif // FORMMAIN_H
