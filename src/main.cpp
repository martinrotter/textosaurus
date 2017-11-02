// For license of this file, see <project-root-folder>/LICENSE.md.

#include "definitions/definitions.h"
#include "dynamic-shortcuts/dynamicshortcuts.h"
#include "gui/dialogs/formabout.h"
#include "gui/dialogs/formmain.h"
#include "gui/dialogs/formupdate.h"
#include "gui/messagebox.h"
#include "miscellaneous/application.h"
#include "miscellaneous/debugging.h"
#include "miscellaneous/iconfactory.h"
#include "miscellaneous/textapplication.h"
#include "network-web/silentnetworkaccessmanager.h"
#include "network-web/webfactory.h"

// Needed for setting ini file format on Mac OS.
#ifdef Q_OS_MAC
#include <QSettings>
#endif

#include <QDebug>
#include <QThread>
#include <QTranslator>

#if defined (Q_OS_MAC)
extern void disableWindowTabbing();

#endif

int main(int argc, char* argv[]) {
  for (int i = 0; i < argc; i++) {
    const QString str = QString::fromLocal8Bit(argv[i]);

    if (str == QL1S("-h")) {
      qDebug("Usage: textilosaurus [OPTIONS] [FILE LIST]\n\n"
             "Option\t\tMeaning\n"
             "-h\t\tDisplays this help.");
      return EXIT_SUCCESS;
    }
  }

  //: Abbreviation of language, e.g. en.
  //: Use ISO 639-1 code here combined with ISO 3166-1 (alpha-2) code.
  //: Examples: "cs", "en", "it", "cs_CZ", "en_GB", "en_US".
  QObject::tr("LANG_ABBREV");

  //: Name of translator - optional.
  QObject::tr("LANG_AUTHOR");

  // Ensure that ini format is used as application settings storage on Mac OS.
  QSettings::setDefaultFormat(QSettings::IniFormat);

  // Setup debug output system.
  qInstallMessageHandler(Debugging::debugHandler);

  // Instantiate base application object.
  Application application(APP_LOW_NAME, argc, argv);

  qDebug("Instantiated Application class.");

  // Check if another instance is running.
  if (application.sendMessage((QStringList() << APP_IS_RUNNING << application.arguments().mid(1)).join(ARGUMENTS_LIST_SEPARATOR))) {
    qWarning("Another instance of the application is already running. Notifying it.");
    return EXIT_FAILURE;
  }

  // Load localization and setup locale before any widget is constructed.
  qApp->localization()->loadActiveLanguage();
  QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
  QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

#if defined (Q_OS_MAC)
  QApplication::setAttribute(Qt::AA_DontShowIconsInMenus);
  disableWindowTabbing();
#endif

  // Add an extra path for non-system icon themes and set current icon theme
  // and skin.
  qApp->icons()->setupSearchPaths();
  qApp->icons()->loadCurrentIconTheme();
  qApp->setStyle(qApp->settings()->value(GROUP(GUI), SETTING(GUI::Style)).toString());

  // These settings needs to be set before any QSettings object.
  Application::setApplicationName(APP_NAME);
  Application::setApplicationVersion(APP_VERSION);
  Application::setOrganizationDomain(APP_URL);
  Application::setWindowIcon(QIcon(APP_ICON_PATH));

  // Setup single-instance behavior.
  QObject::connect(&application, &Application::messageReceived, &application, &Application::processExecutionMessage);
  qDebug().nospace() << "Creating main application form in thread: \'" << QThread::currentThreadId() << "\'.";

  // Instantiate main application window.
  FormMain main_window;

  // Set correct information for main window.
  main_window.setWindowTitle(APP_LONG_NAME);

  // Now is a good time to initialize dynamic keyboard shortcuts.
  DynamicShortcuts::load(qApp->userActions());

  // Display main window.
  qDebug("Showing the main window when the application is starting.");
  main_window.show();

  if (true || qApp->isFirstRun() || qApp->isFirstRun(APP_VERSION)) {
    qApp->showGuiMessage(QObject::tr("Welcome to %1. Please, check NEW stuff included in this "
                                     "version by going to Help → About application → Changelog.").arg(APP_LONG_NAME));
  }

  DynamicShortcuts::load(qApp->userActions());

  if (qApp->settings()->value(GROUP(General), SETTING(General::UpdateOnStartup)).toBool()) {
    QObject::connect(qApp->system(), &SystemFactory::updatesChecked, [](QPair<QList<UpdateInfo>, QNetworkReply::NetworkError> updates) {
      QObject::disconnect(qApp->system(), &SystemFactory::updatesChecked, nullptr, nullptr);

      if (!updates.first.isEmpty() && updates.second == QNetworkReply::NoError &&
          SystemFactory::isVersionNewer(updates.first.at(0).m_availableVersion, APP_VERSION)) {
        qApp->showGuiMessage(QObject::tr("There is a new version of %1 available").arg(APP_NAME), QMessageBox::Icon::Information);
      }
    });
    qApp->system()->checkForUpdates();
  }

  // We load any documents passed as parameters.
  qApp->textApplication()->loadFilesFromArgs(argc, argv);

  // Enter global event loop.
  return Application::exec();
}
