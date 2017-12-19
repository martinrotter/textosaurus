// For license of this file, see <project-root-folder>/LICENSE.md.

#include "gui/dialogs/formmain.h"

#include "definitions/definitions.h"
#include "gui/dialogs/formabout.h"
#include "gui/dialogs/formfindreplace.h"
#include "gui/dialogs/formsettings.h"
#include "gui/dialogs/formupdate.h"
#include "gui/messagebox.h"
#include "gui/plaintoolbutton.h"
#include "gui/statusbar.h"
#include "gui/tabbar.h"
#include "gui/tabwidget.h"
#include "gui/toolbar.h"
#include "miscellaneous/application.h"
#include "miscellaneous/iconfactory.h"
#include "miscellaneous/settings.h"
#include "miscellaneous/systemfactory.h"
#include "miscellaneous/textapplication.h"
#include "network-web/webfactory.h"

#include <QCloseEvent>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QMetaObject>
#include <QRect>
#include <QScopedPointer>
#include <QTimer>

FormMain::FormMain(QWidget* parent) : QMainWindow(parent), m_statusBar() {
  m_ui.setupUi(this);

  qApp->setMainForm(this);

  setCentralWidget(m_tabEditors = new TabWidget(this));
  setStatusBar(m_statusBar = new StatusBar(this));
  addToolBar(m_toolBar = new ToolBar(tr("Main toolbar"), this));

  m_statusBar->setObjectName(QSL("m_statusBar"));
  m_tabEditors->setObjectName(QSL("m_tabEditors"));
  m_toolBar->setObjectName(QSL("m_toolBar"));

  qApp->textApplication()->setMainForm(this);

  // Prepare main window and tabs.
  prepareMenus();

  // Establish connections.
  createConnections();

  // Setup some appearance of the window.
  setupIcons();
  loadSize();
}

TabWidget* FormMain::tabWidget() const {
  return m_tabEditors;
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
  actions << m_ui.m_actionFileReload;

#if !defined(Q_OS_MAC)
  actions << m_ui.m_actionFullscreen;
#endif

  actions << m_ui.m_actionEditBack;
  actions << m_ui.m_actionEditForward;
  actions << m_ui.m_actionEolConvertMac;
  actions << m_ui.m_actionEolConvertUnix;
  actions << m_ui.m_actionEolConvertWindows;
  actions << m_ui.m_actionEolMac;
  actions << m_ui.m_actionEolUnix;
  actions << m_ui.m_actionEolWindows;
  actions << m_ui.m_actionFindReplace;
  actions << m_ui.m_actionLineNumbers;
  actions << m_ui.m_actionWordWrap;
  actions << m_ui.m_actionViewEols;
  actions << m_ui.m_actionViewWhitespaces;
  actions << m_ui.m_actionDockShowOutput;
  actions << m_ui.m_actionDockShowFilesystem;

  actions << m_ui.m_actionAboutGuard;
  actions << m_ui.m_actionSwitchMainWindow;
  actions << m_ui.m_actionSwitchStatusBar;
  actions << m_ui.m_actionTabsNext;
  actions << m_ui.m_actionTabsPrevious;
  actions << m_ui.m_actionTabsCloseAll;
  actions << m_ui.m_actionTabsCloseCurrent;
  actions << m_ui.m_actionTabsCloseAllExceptCurrent;
  actions << m_ui.m_actionTabsCloseAllUnmodified;

  return actions;
}

void FormMain::prepareMenus() {
#if defined(Q_OS_MAC)
  m_ui.m_actionFullscreen->setVisible(false);
#endif
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

void FormMain::dragEnterEvent(QDragEnterEvent* event) {
  event->accept();
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
  setWindowState((windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
  show();
  activateWindow();
  raise();

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
  m_ui.m_menuFileReopenWithEncoding->setIcon(icon_theme_factory->fromTheme(QSL("document-open")));
  m_ui.m_actionFileSave->setIcon(icon_theme_factory->fromTheme(QSL("document-save")));
  m_ui.m_actionFileSaveAs->setIcon(icon_theme_factory->fromTheme(QSL("document-save-as")));
  m_ui.m_actionFileSaveAll->setIcon(icon_theme_factory->fromTheme(QSL("document-save")));
  m_ui.m_menuFileSaveWithEncoding->setIcon(icon_theme_factory->fromTheme(QSL("document-save-as")));

  // Edit.
  m_ui.m_actionEditBack->setIcon(icon_theme_factory->fromTheme(QSL("edit-undo")));
  m_ui.m_actionEditForward->setIcon(icon_theme_factory->fromTheme(QSL("edit-redo")));

  // View.
  m_ui.m_actionSwitchMainWindow->setIcon(icon_theme_factory->fromTheme(QSL("window-close")));
  m_ui.m_actionFullscreen->setIcon(icon_theme_factory->fromTheme(QSL("view-fullscreen")));
  m_ui.m_actionSwitchStatusBar->setIcon(icon_theme_factory->fromTheme(QSL("dialog-information")));

  // Tabs & web browser.
  m_ui.m_actionTabsCloseAll->setIcon(icon_theme_factory->fromTheme(QSL("window-close")));
  m_ui.m_actionTabsCloseAllExceptCurrent->setIcon(icon_theme_factory->fromTheme(QSL("window-close")));
  m_ui.m_actionTabsCloseAllUnmodified->setIcon(icon_theme_factory->fromTheme(QSL("window-close")));
  m_ui.m_actionTabsCloseCurrent->setIcon(icon_theme_factory->fromTheme(QSL("window-close")));
  m_ui.m_actionTabsNext->setIcon(icon_theme_factory->fromTheme(QSL("go-next")));
  m_ui.m_actionTabsPrevious->setIcon(icon_theme_factory->fromTheme(QSL("go-previous")));
}

void FormMain::loadSize() {
  const QRect screen = qApp->desktop()->screenGeometry();
  const Settings* settings = qApp->settings();

  // Reload main window size & position.
  resize(settings->value(GROUP(GUI), GUI::MainWindowInitialSize, size()).toSize());
  move(settings->value(GROUP(GUI), GUI::MainWindowInitialPosition, screen.center() - rect().center()).toPoint());

  if (!settings->value(GROUP(GUI), SETTING(GUI::ToolbarsVisible)).toBool()) {
    toolBar()->hide();
  }

  if (!settings->value(GROUP(GUI), SETTING(GUI::StatusBarVisible)).toBool()) {
    statusBar()->hide();
  }

  restoreState(settings->value(GROUP(GUI), SETTING(GUI::MainWindowState)).toByteArray());
}

void FormMain::saveSize() {
  Settings* settings = qApp->settings();
  bool is_fullscreen = isFullScreen();

  if (is_fullscreen) {
    switchFullscreenMode();

    // We (process events to really) un-fullscreen, so that we can determine if window is really maximized.
    qApp->processEvents();
  }

  if (isMaximized()) {
    showNormal();

    // We process events to really have window un-maximized.
    qApp->processEvents();
  }

  settings->setValue(GROUP(GUI), GUI::MainWindowInitialPosition, pos());
  settings->setValue(GROUP(GUI), GUI::MainWindowInitialSize, size());
  settings->setValue(GROUP(GUI), GUI::ToolbarsVisible, toolBar()->isActive());
  settings->setValue(GROUP(GUI), GUI::StatusBarVisible, statusBar()->isActive());
  settings->setValue(GROUP(GUI), GUI::MainWindowState, saveState());
}

void FormMain::createConnections() {
  // Menu "File" connections.
  connect(m_ui.m_actionQuit, &QAction::triggered, qApp, &Application::quitApplication);
  connect(m_ui.m_actionRestart, &QAction::triggered, this, [this]() {
    if (close()) {
      qApp->restart();
    }
  });

  // Menu "View" connections.
  connect(m_ui.m_menuShowHide, &QMenu::aboutToShow, [this]() {
    m_ui.m_actionFullscreen->setChecked(isFullScreen());
    m_ui.m_actionSwitchStatusBar->setChecked(statusBar()->isVisible());
    m_ui.m_actionSwitchToolBar->setChecked(toolBar()->isVisible());
  });
  connect(m_ui.m_actionFullscreen, &QAction::triggered, this, &FormMain::switchFullscreenMode);
  connect(m_ui.m_actionSwitchMainWindow, &QAction::triggered, this, &FormMain::switchVisibility);
  connect(m_ui.m_actionSwitchToolBar, &QAction::triggered, toolBar(), &ToolBar::setIsActive);
  connect(m_ui.m_actionSwitchStatusBar, &QAction::triggered, statusBar(), &StatusBar::setIsActive);

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
  connect(m_ui.m_actionTabsNext, &QAction::triggered, m_tabEditors, &TabWidget::gotoNextTab);
  connect(m_ui.m_actionTabsPrevious, &QAction::triggered, m_tabEditors, &TabWidget::gotoPreviousTab);
  connect(m_ui.m_actionTabsCloseAllExceptCurrent, &QAction::triggered, m_tabEditors, &TabWidget::closeAllTabsExceptCurrent);
  connect(m_ui.m_actionTabsCloseAll, &QAction::triggered, m_tabEditors, &TabWidget::closeAllTabs);
  connect(m_ui.m_actionTabsCloseCurrent, &QAction::triggered, m_tabEditors, &TabWidget::closeCurrentTab);
}

void FormMain::showWiki() {
  if (!qApp->web()->openUrlInExternalBrowser(APP_URL_WIKI)) {
    qApp->showGuiMessage(tr("Cannot open external browser. Navigate to application website manually."),
                         QMessageBox::Warning);
  }
}

void FormMain::reportABug() {
  if (!qApp->web()->openUrlInExternalBrowser(QSL(APP_URL_ISSUES_NEW))) {
    qApp->showGuiMessage(tr("Cannot open external browser. Navigate to application website manually."),
                         QMessageBox::Warning);
  }
}

void FormMain::donate() {
  if (!qApp->web()->openUrlInExternalBrowser(QSL(APP_DONATE_URL))) {
    qApp->showGuiMessage(tr("Cannot open external browser. Navigate to application website manually."),
                         QMessageBox::Warning);
  }
}
