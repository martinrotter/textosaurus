// For license of this file, see <project-root-folder>/LICENSE.md.

#ifdef MAKING_LIBRARY
#undef MAKING_LIBRARY
#endif

#include "common/dynamic-shortcuts/dynamicshortcuts.h"
#include "common/gui/messagebox.h"
#include "common/gui/systemtrayicon.h"
#include "common/miscellaneous/debugging.h"
#include "common/miscellaneous/iconfactory.h"
#include "common/network-web/silentnetworkaccessmanager.h"
#include "common/network-web/webfactory.h"
#include "definitions/definitions.h"
#include "saurus/gui/dialogs/formmain.h"
#include "saurus/miscellaneous/application.h"
#include "saurus/miscellaneous/textapplication.h"

#include <QDebug>

int main(int argc, char* argv[]) {
  // Setup debug output system.
  qInstallMessageHandler(Debugging::debugHandler);

  QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
  QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QApplication::setApplicationName(APP_NAME);
  QApplication::setApplicationVersion(APP_VERSION);
  QApplication::setOrganizationDomain(APP_URL);

  // Instantiate base application object.
  Application application(APP_LOW_NAME, argc, argv);

  // Check if another instance is running.
  if (application.isRunning()) {
    qWarning("Another instance of the application is already running. Notifying it.");
    return EXIT_FAILURE;
  }

#if defined (FLATPAK_MODE)
  qDebug("Flatpak mode enabled.");
#endif

  QApplication::setWindowIcon(QIcon(APP_ICON_PATH));

#if defined (Q_OS_MAC)
  QApplication::setAttribute(Qt::AA_DontShowIconsInMenus);
#endif

  qApp->icons()->setupSearchPaths();
  qApp->icons()->loadIconTheme(qApp->settings()->value(GROUP(GUI), SETTING(GUI::IconTheme)).toString());
  qApp->localization()->loadActiveLanguage();
  qApp->setStyle(qApp->settings()->value(GROUP(GUI), SETTING(GUI::Style)).toString());

  // Setup single-instance behavior.
  application.activateQtSingleMsgProcessing();

  // Instantiate main application window.
  FormMain main_window;

  // Set correct information for main window.
  main_window.show();

  // Restore opened documents and open passed documents
  // if any.
  qApp->textApplication()->restoreSession();
  qApp->textApplication()->openPassedFilesOrNewDocument();

  // Display tray icon if it is enabled and available.
  if (SystemTrayIcon::isSystemTrayActivated()) {
    qApp->showTrayIcon();
  }

  // Now is a good time to initialize dynamic keyboard shortcuts.
  DynamicShortcuts::load(qApp->userActions());

  // Enter global event loop.
  return Application::exec();
}
