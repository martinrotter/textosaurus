// For license of this file, see <project-root-folder>/LICENSE.md.

#include "gui/dialogs/formmain.h"

#include "definitions/definitions.h"
#include "gui/dialogs/formabout.h"
#include "gui/dialogs/formsettings.h"
#include "gui/dialogs/formupdate.h"
#include "gui/messagebox.h"
#include "gui/plaintoolbutton.h"
#include "gui/statusbar.h"
#include "gui/tabbar.h"
#include "gui/toolbar.h"
#include "miscellaneous/application.h"
#include "miscellaneous/iconfactory.h"
#include "miscellaneous/mutex.h"
#include "miscellaneous/settings.h"
#include "miscellaneous/systemfactory.h"
#include "miscellaneous/textapplication.h"
#include "network-web/webfactory.h"

#include <QCloseEvent>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QRect>
#include <QScopedPointer>
#include <QTimer>

FormMain::FormMain(QWidget* parent) : QMainWindow(parent), m_statusBar() {
  m_ui.setupUi(this);
  qApp->setMainForm(this);

  // Add these actions to the list of actions of the main window.
  // This allows to use actions via shortcuts
  // even if main menu is not visible.
  addActions(qApp->userActions());
  setStatusBar(m_statusBar = new StatusBar(this));
  addToolBar(m_toolBar = new ToolBar(tr("Main toolbar"), this));

  qApp->textApplication()->setMainForm(this, tabWidget(), statusBar());

  // Prepare main window and tabs.
  prepareMenus();

  // Establish connections.
  createConnections();

  // Setup some appearance of the window.
  setupIcons();
  loadSize();
}

FormMain::~FormMain() {
  qDebug("Destroying FormMain instance.");
}

TabWidget* FormMain::tabWidget() const {
  return m_ui.m_tabEditors;
}

ToolBar* FormMain::toolBar() const {
  return m_toolBar;
}

StatusBar* FormMain::statusBar() const {
  return m_statusBar;
}

QList<QAction*> FormMain::allActions() const {
  QList<QAction*> actions;

  // Add basic actions.
  actions << m_ui.m_actionSettings;
  actions << m_ui.m_actionRestart;
  actions << m_ui.m_actionQuit;
  actions << m_ui.m_actionFileNew;
  actions << m_ui.m_actionFileOpen;
  actions << m_ui.m_actionFileSave;
  actions << m_ui.m_actionFileSaveAll;
  actions << m_ui.m_actionFileSaveAs;

#if !defined(Q_OS_MAC)
  actions << m_ui.m_actionFullscreen;
#endif

  actions << m_ui.m_actionEditBack;
  actions << m_ui.m_actionEditForward;

  actions << m_ui.m_actionAboutGuard;
  actions << m_ui.m_actionSwitchMainWindow;
  actions << m_ui.m_actionSwitchStatusBar;
  actions << m_ui.m_actionTabsNext;
  actions << m_ui.m_actionTabsPrevious;
  actions << m_ui.m_actionTabsCloseAll;
  actions << m_ui.m_actionTabsCloseAllExceptCurrent;
  actions << m_ui.m_actionTabsCloseAllUnmodified;

  return actions;
}

void FormMain::prepareMenus() {
#if defined(Q_OS_MAC)
  m_ui.m_actionFullscreen->setVisible(false);
#endif

  QActionGroup* grp_eol = new QActionGroup(m_ui.m_menuEolMode);

  grp_eol->addAction(m_ui.m_actionEolMac);
  grp_eol->addAction(m_ui.m_actionEolUnix);
  grp_eol->addAction(m_ui.m_actionEolWindows);
  grp_eol->setExclusive(true);

  //m_ui.m_menuEolMode->addAction(grp_eol);
}

void FormMain::switchFullscreenMode() {
  if (!isFullScreen()) {
    qApp->settings()->setValue(GROUP(GUI), GUI::IsMainWindowMaximizedBeforeFullscreen, isMaximized());
    showFullScreen();
  }
  else {
    if (qApp->settings()->value(GROUP(GUI), SETTING(GUI::IsMainWindowMaximizedBeforeFullscreen)).toBool()) {
      setWindowState((windowState() & ~Qt::WindowFullScreen) | Qt::WindowMaximized);
    }
    else {
      showNormal();
    }
  }
}

void FormMain::closeEvent(QCloseEvent* event) {
  bool should_stop = true;
  emit closeRequested(&should_stop);

  if (should_stop) {
    event->accept();
  }
  else {
    event->ignore();
  }
}

void FormMain::switchVisibility() {
  if (isVisible()) {
    showMinimized();
  }
  else {
    display();
  }
}

void FormMain::display() {
  // Display the window and make sure it is raised on top.
  show();
  activateWindow();
  raise();

  // Make sure window is not minimized.
  setWindowState((windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);

  // Raise alert event. Check the documentation for more info on this.
  Application::alert(this);
}

void FormMain::setupIcons() {
  IconFactory* icon_theme_factory = qApp->icons();

  // Setup icons of this main window.
  m_ui.m_actionSettings->setIcon(icon_theme_factory->fromTheme(QSL("document-properties")));
  m_ui.m_actionQuit->setIcon(icon_theme_factory->fromTheme(QSL("application-exit")));
  m_ui.m_actionRestart->setIcon(icon_theme_factory->fromTheme(QSL("view-refresh")));
  m_ui.m_actionAboutGuard->setIcon(icon_theme_factory->fromTheme(QSL("help-about")));
  m_ui.m_actionCheckForUpdates->setIcon(icon_theme_factory->fromTheme(QSL("system-upgrade")));
  m_ui.m_actionReportBug->setIcon(icon_theme_factory->fromTheme(QSL("call-start")));
  m_ui.m_actionDonate->setIcon(icon_theme_factory->fromTheme(QSL("applications-office")));
  m_ui.m_actionDisplayWiki->setIcon(icon_theme_factory->fromTheme(QSL("applications-science")));

  m_ui.m_actionFileNew->setIcon(icon_theme_factory->fromTheme(QSL("document-new")));
  m_ui.m_actionFileOpen->setIcon(icon_theme_factory->fromTheme(QSL("document-open")));
  m_ui.m_menuFileOpenWithEncoding->setIcon(icon_theme_factory->fromTheme(QSL("document-open")));
  m_ui.m_actionFileSave->setIcon(icon_theme_factory->fromTheme(QSL("document-save")));
  m_ui.m_actionFileSaveAs->setIcon(icon_theme_factory->fromTheme(QSL("document-save-as")));
  m_ui.m_actionFileSaveAll->setIcon(icon_theme_factory->fromTheme(QSL("document-save")));
  m_ui.m_menuFileSaveWithEncoding->setIcon(icon_theme_factory->fromTheme(QSL("document-save-as")));

  // Edit.
  m_ui.m_actionEditBack->setIcon(icon_theme_factory->fromTheme(QSL("back")));
  m_ui.m_actionEditForward->setIcon(icon_theme_factory->fromTheme(QSL("forward")));

  // View.
  m_ui.m_actionSwitchMainWindow->setIcon(icon_theme_factory->fromTheme(QSL("window-close")));
  m_ui.m_actionFullscreen->setIcon(icon_theme_factory->fromTheme(QSL("view-fullscreen")));
  m_ui.m_actionSwitchStatusBar->setIcon(icon_theme_factory->fromTheme(QSL("dialog-information")));

  // Tabs & web browser.
  m_ui.m_actionTabsCloseAll->setIcon(icon_theme_factory->fromTheme(QSL("window-close")));
  m_ui.m_actionTabsCloseAllExceptCurrent->setIcon(icon_theme_factory->fromTheme(QSL("window-close")));
  m_ui.m_actionTabsCloseAllUnmodified->setIcon(icon_theme_factory->fromTheme(QSL("window-close")));
  m_ui.m_actionTabsNext->setIcon(icon_theme_factory->fromTheme(QSL("go-next")));
  m_ui.m_actionTabsPrevious->setIcon(icon_theme_factory->fromTheme(QSL("go-previous")));
}

void FormMain::loadSize() {
  const QRect screen = qApp->desktop()->screenGeometry();
  const Settings* settings = qApp->settings();

  // Reload main window size & position.
  resize(settings->value(GROUP(GUI), GUI::MainWindowInitialSize, size()).toSize());
  move(settings->value(GROUP(GUI), GUI::MainWindowInitialPosition, screen.center() - rect().center()).toPoint());

  if (settings->value(GROUP(GUI), SETTING(GUI::MainWindowStartsMaximized)).toBool()) {
    setWindowState(windowState() | Qt::WindowMaximized);

    // We process events so that window is really maximized fast.
    qApp->processEvents();
  }

  // If user exited the application while in fullsreen mode,
  // then re-enable it now.
  if (settings->value(GROUP(GUI), SETTING(GUI::MainWindowStartsFullscreen)).toBool()) {
    m_ui.m_actionFullscreen->setChecked(true);
  }

  m_ui.m_actionSwitchToolBar->setChecked(settings->value(GROUP(GUI), SETTING(GUI::ToolbarsVisible)).toBool());
  m_ui.m_actionSwitchStatusBar->setChecked(settings->value(GROUP(GUI), SETTING(GUI::StatusBarVisible)).toBool());

  m_ui.m_splitter->restoreState(settings->value(GROUP(GUI), GUI::MainWindowSplitter, m_ui.m_splitter->saveState()).toByteArray());
}

void FormMain::saveSize() {
  Settings* settings = qApp->settings();
  bool is_fullscreen = isFullScreen();
  bool is_maximized = false;

  settings->setValue(GROUP(GUI), GUI::MainWindowSplitter, m_ui.m_splitter->saveState());

  if (is_fullscreen) {
    m_ui.m_actionFullscreen->setChecked(false);

    // We (process events to really) un-fullscreen, so that we can determine if window is really maximized.
    qApp->processEvents();
  }

  if (isMaximized()) {
    is_maximized = true;

    // Window is maximized, we store that fact to settings and unmaximize.
    qApp->settings()->setValue(GROUP(GUI), GUI::IsMainWindowMaximizedBeforeFullscreen, isMaximized());
    setWindowState((windowState() & ~Qt::WindowMaximized) | Qt::WindowActive);

    // We process events to really have window un-maximized.
    qApp->processEvents();
  }

  settings->setValue(GROUP(GUI), GUI::MainWindowInitialPosition, pos());
  settings->setValue(GROUP(GUI), GUI::MainWindowInitialSize, size());
  settings->setValue(GROUP(GUI), GUI::MainWindowStartsMaximized, is_maximized);
  settings->setValue(GROUP(GUI), GUI::MainWindowStartsFullscreen, is_fullscreen);

  settings->setValue(GROUP(GUI), GUI::ToolbarsVisible, m_ui.m_actionSwitchToolBar->isChecked());
  settings->setValue(GROUP(GUI), GUI::StatusBarVisible, m_ui.m_actionSwitchStatusBar->isChecked());
}

void FormMain::createConnections() {
  // Menu "File" connections.
  connect(m_ui.m_actionQuit, &QAction::triggered, this, &FormMain::close);
  connect(m_ui.m_actionRestart, &QAction::triggered, this, [this]() {
    if (close()) {
      qApp->restart();
    }
  });

  // Menu "View" connections.
  connect(m_ui.m_actionFullscreen, &QAction::toggled, this, &FormMain::switchFullscreenMode);
  connect(m_ui.m_actionSwitchMainWindow, &QAction::triggered, this, &FormMain::switchVisibility);
  connect(m_ui.m_actionSwitchToolBar, &QAction::toggled, toolBar(), &ToolBar::setVisible);
  connect(m_ui.m_actionSwitchStatusBar, &QAction::toggled, statusBar(), &StatusBar::setVisible);

  // Menu "Tools" connections.
  connect(m_ui.m_actionSettings, &QAction::triggered, this, [this]() {
    FormSettings(*this).exec();
  });

  // Menu "Help" connections.
  connect(m_ui.m_actionAboutGuard, &QAction::triggered, this, [this]() {
    FormAbout(this).exec();
  });
  connect(m_ui.m_actionCheckForUpdates, &QAction::triggered, this, [this]() {
    FormUpdate(this).exec();
  });
  connect(m_ui.m_actionReportBug, &QAction::triggered, this, &FormMain::reportABug);
  connect(m_ui.m_actionDonate, &QAction::triggered, this, &FormMain::donate);
  connect(m_ui.m_actionDisplayWiki, &QAction::triggered, this, &FormMain::showWiki);

  // Tab widget connections.
  connect(m_ui.m_actionTabsNext, &QAction::triggered, m_ui.m_tabEditors, &TabWidget::gotoNextTab);
  connect(m_ui.m_actionTabsPrevious, &QAction::triggered, m_ui.m_tabEditors, &TabWidget::gotoPreviousTab);
  connect(m_ui.m_actionTabsCloseAllExceptCurrent, &QAction::triggered, m_ui.m_tabEditors, &TabWidget::closeAllTabsExceptCurrent);
  connect(m_ui.m_actionTabsCloseAll, &QAction::triggered, m_ui.m_tabEditors, &TabWidget::closeAllTabs);
}

void FormMain::showWiki() {
  if (!qApp->web()->openUrlInExternalBrowser(APP_URL_WIKI)) {
    qApp->showGuiMessage(tr("Cannot open external browser"),
                         tr("Cannot open external browser. Navigate to application website manually."),
                         QMessageBox::Warning, this, true);
  }
}

void FormMain::reportABug() {
  if (!qApp->web()->openUrlInExternalBrowser(QSL(APP_URL_ISSUES_NEW))) {
    qApp->showGuiMessage(tr("Cannot open external browser"),
                         tr("Cannot open external browser. Navigate to application website manually."),
                         QMessageBox::Warning, this, true);
  }
}

void FormMain::donate() {
  if (!qApp->web()->openUrlInExternalBrowser(QSL(APP_DONATE_URL))) {
    qApp->showGuiMessage(tr("Cannot open external browser"),
                         tr("Cannot open external browser. Navigate to application website manually."),
                         QMessageBox::Warning, this, true);
  }
}
