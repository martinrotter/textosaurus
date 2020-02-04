// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifdef MAKING_LIBRARY
#undef MAKING_LIBRARY
#endif

#define MAIN_CODE

#include "common/dynamic-shortcuts/dynamicshortcuts.h"
#include "common/gui/systemtrayicon.h"
#include "common/gui/toolbar.h"
#include "common/miscellaneous/debugging.h"
#include "common/miscellaneous/iconfactory.h"
#include "saurus/gui/dialogs/formmain.h"
#include "saurus/gui/statusbar.h"
#include "saurus/miscellaneous/application.h"
#include "saurus/miscellaneous/textapplication.h"

#include <QCommandLineParser>
#include <QDebug>

int main(int argc, char* argv[]) {
  // Setup debug output system.
  qInstallMessageHandler(Debugging::debugHandler);

  QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
  QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QCoreApplication::setApplicationName(APP_NAME);
  QCoreApplication::setApplicationVersion(APP_VERSION);
  QCoreApplication::setOrganizationDomain(APP_URL);

  // Instantiate base application object.
  Application application(APP_LOW_NAME, argc, argv);

  // Check if another instance is running.
  if (application.forcesNewInstance()) {
    qWarningNN << QSL("New instance of application was forced by the user.");
  }
  else if (application.isRunning()) {
    qWarning().noquote().nospace() << QSL("Another instance of the application is already running. Notifying it.");
    return EXIT_FAILURE;
  }

#if defined (FLATPAK_MODE)
  qDebug().noquote().nospace() << QSL("Flatpak mode enabled.");
#endif

  QApplication::setWindowIcon(QIcon(APP_ICON_PATH));

#if defined (Q_OS_MAC)
  QCoreApplication::setAttribute(Qt::AA_DontShowIconsInMenus);
#endif

  qApp->settings()->printSettingsInfo();
  qApp->icons()->setupSearchPaths();
  qApp->icons()->loadIconTheme(qApp->settings()->value(GROUP(GUI), SETTING(GUI::IconTheme)).toString());
  qApp->localization()->loadActiveLanguage();
  qApp->setStyle(qApp->settings()->value(GROUP(GUI), SETTING(GUI::Style)).toString());

  // Initialize "text application" core object after all supporting facilities (localizations, icons, ...)
  // are ready.
  qApp->setTextApplication(new TextApplication(qApp));

  // Setup single-instance behavior.
  if (!application.forcesNewInstance()) {
    application.activateQtSingleMsgProcessing();
  }

  // Instantiate main application window.
  FormMain main_window;

  qApp->textApplication()->setMainForm(&main_window);

  // Load saved actions, this needs to be called AFTER
  // plugins are loaded, because plugins
  // can contain actions too.
  main_window.toolBar()->loadSavedActions();

  // Load state of main window, this includes visible sidebars etc.
  // This also needs to be loaded AFTER plugins are
  // loaded because plugins can contain sidebars.
  main_window.loadSize();

  // Set correct information for main window.
  main_window.show();

  // Restore opened documents and open passed documents
  // if any.
  qApp->textApplication()->restoreSession();

  // TODO: Předělat, aby používalo cmdparser z application?
  qApp->textApplication()->processCommandLineArguments();

  // Display tray icon if it is enabled and available.
  if (SystemTrayIcon::isSystemTrayActivated()) {
    qApp->showTrayIcon();
  }

  // Now is a good time to initialize dynamic keyboard shortcuts.
  DynamicShortcuts::load(qApp->userActions(), qApp->settings());

  // Enter global event loop.
  return QApplication::exec();
}
