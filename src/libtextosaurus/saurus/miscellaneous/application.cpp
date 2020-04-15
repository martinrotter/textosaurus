// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "saurus/miscellaneous/application.h"

#include "common/exceptions/applicationexception.h"
#include "common/gui/messagebox.h"
#include "common/gui/systemtrayicon.h"
#include "common/miscellaneous/iconfactory.h"
#include "common/miscellaneous/iofactory.h"
#include "common/network-web/webfactory.h"
#include "saurus/external-tools/externaltools.h"
#include "saurus/gui/dialogs/formmain.h"
#include "saurus/gui/sidebars/outputsidebar.h"
#include "saurus/gui/statusbar.h"
#include "saurus/miscellaneous/textapplication.h"
#include "saurus/plugin-system/pluginfactory.h"

#include <QProcess>
#include <QSessionManager>

Application::Application(const QString& id, int& argc, char** argv)
  : QtSingleApplication(id, argc, argv),
  m_settings(nullptr),
  m_textApplication(nullptr), m_mainForm(nullptr),
  m_webFactory(new WebFactory(this)),
  m_system(new SystemFactory(this)),
  m_localization(new Localization(this)), m_icons(new IconFactory(this)),
  m_trayIcon(nullptr),
  m_shouldRestart(false),
  m_isQuitting(false) {

  // Parse cmd arguments and then initialize settings.
  parseCmdArguments();
  m_settings = Settings::setupSettings(this,
                                       qApp->userDataAppFolder(),
                                       qApp->userDataHomeFolder(),
                                       m_cmdParser.value(APP_OPT_CONFIG_SHORT));

  //: Abbreviation of language, e.g. en.
  //: Use ISO 639-1 code here combined with ISO 3166-1 (alpha-2) code.
  //: Examples: "cs", "en", "it", "cs_CZ", "en_GB", "en_US".
  QObject::tr("LANG_ABBREV");

  connect(this, &Application::aboutToQuit, this, &Application::onAboutToQuit);
  connect(this, &Application::commitDataRequest, this, &Application::onCommitData);
  connect(this, &Application::saveStateRequest, this, &Application::onSaveState);

  setQuitOnLastWindowClosed(true);
}

void Application::activateQtSingleMsgProcessing() {
  connect(this, &Application::messageReceived, this, &Application::processExecutionMessage);
}

bool Application::isRunning() {
  return sendMessage((QStringList() << QString("-%1").arg(APP_IS_RUNNING_SHORT) << arguments().mid(1)).join(ARGUMENTS_LIST_SEPARATOR));
}

bool Application::forcesNewInstance() const {
  return m_cmdParser.isSet(APP_NOSINGLE_INSTANCE_SHORT);
}

bool Application::forcesNativeDialogs() const {
  return m_cmdParser.isSet(APP_NAT_DLGS_CONFIG);
}

QList<QAction*> Application::userActions() {
  QList<QAction*> user_actions;

  if (m_mainForm != nullptr) {
    user_actions = m_mainForm->allActions();
  }

  if (m_textApplication != nullptr) {
    user_actions.append(m_textApplication->userActions());
  }

  return user_actions;
}

bool Application::isFirstRun() const {
  return settings()->value(GROUP(General), SETTING(General::FirstRun)).toBool();
}

bool Application::isFirstRun(const QString& version) const {
  if (version == APP_VERSION) {
    // Check this only if checked version is equal to actual version.
    return settings()->value(GROUP(General), QString(General::FirstRun) + QL1C('_') + version, true).toBool();
  }
  else {
    return false;
  }
}

TextApplication* Application::textApplication() const {
  return m_textApplication;
}

WebFactory* Application::web() const {
  return m_webFactory;
}

SystemFactory* Application::system() {
  return m_system;
}

Localization* Application::localization() {
  return m_localization;
}

void Application::eliminateFirstRun() {
  settings()->setValue(GROUP(General), General::FirstRun, false);
}

void Application::eliminateFirstRun(const QString& version) {
  settings()->setValue(GROUP(General), QString(General::FirstRun) + QL1C('_') + version, false);
}

bool Application::isQuitting() const {
  return m_isQuitting;
}

IconFactory* Application::icons() {
  return m_icons;
}

Settings* Application::settings() const {
  return m_settings;
}

FormMain* Application::mainForm() {
  return m_mainForm;
}

QWidget* Application::mainFormWidget() {
  return m_mainForm;
}

void Application::showTrayIcon() {
  qDebug().noquote() << QSL("Showing tray icon.");
  trayIcon()->show();
}

void Application::deleteTrayIcon() {
  if (m_trayIcon != nullptr) {
    qDebug().noquote() << QSL("Disabling tray icon, deleting it and raising main application window.");
    m_mainForm->display();
    delete m_trayIcon;
    m_trayIcon = nullptr;

    // Make sure that application quits when last window is closed.
    setQuitOnLastWindowClosed(true);
  }
}

SystemTrayIcon* Application::trayIcon() {
  if (m_trayIcon == nullptr) {
#if defined(Q_OS_WIN)
    QMenu* tray_menu = new TrayIconMenu(APP_NAME, m_mainForm);
#else
    QMenu* tray_menu = new QMenu(APP_NAME, m_mainForm);
#endif

    // Ensure that main window is shown before the command
    // from tray icon menu is ran.
    connect(tray_menu, &QMenu::triggered, this, [this]() {
      m_mainForm->display();
    });

    auto* separator = new QAction(tray_menu);

    separator->setSeparator(true);

    tray_menu->addActions(QList<QAction*> {
      m_mainForm->m_ui.m_actionFileNew,
      m_mainForm->m_ui.m_actionFileOpen,
      separator,
      m_mainForm->m_ui.m_actionQuit
    });

    m_trayIcon = new SystemTrayIcon(
      APP_ICON_PATH,
      tray_menu,
      [this]() {
      m_mainForm->switchVisibility();
    },
      m_mainForm);
    m_trayIcon->setToolTip(APP_LONG_NAME);
  }

  return m_trayIcon;
}

QCommandLineParser* Application::cmdParser() {
  return &m_cmdParser;
}

void Application::setMainForm(FormMain* main_form) {
  m_mainForm = main_form;
}

QString Application::configFolder() {
  return QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
}

QString Application::userDataAppFolder() {
  // In "app" folder, we would like to separate all user data into own subfolder,
  // therefore stick to "data" folder in this mode.
  return applicationDirPath() + QDir::separator() + QSL("data");
}

QString Application::userDataFolder() {
  return settings()->baseFolder();
}

QString Application::userDataHomeFolder() {
#if defined (Q_OS_ANDROID)
  return IOFactory::getSystemFolder(QStandardPaths::AppDataLocation);
#else
  return configFolder();
#endif
}

QString Application::userDataForcedFolder() {
  return m_cmdParser.value(APP_OPT_CONFIG_SHORT);
}

void Application::setTextApplication(TextApplication* text_application) {
  m_textApplication = text_application;
}

QString Application::homeFolder() {
  return QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
}

void Application::parseCmdArguments() {
  QCommandLineOption opt_already_running({APP_IS_RUNNING_SHORT, APP_IS_RUNNING},
                                         QSL("Indicates that another application instance is running. You should never need to use this."));
  QCommandLineOption opt_quit({APP_QUIT_INSTANCE_SHORT, APP_QUIT_INSTANCE},
                              QSL("Quit currently running application instance."));
  QCommandLineOption opt_config({APP_OPT_CONFIG_SHORT, APP_OPT_CONFIG},
                                QSL("Use an alternate configuration directory. If the directory does not exist, then it is created. "
                                    "Make sure you use absolute directory path which is writable by the current user."),
                                QSL("absolute-directory-path"));
  QCommandLineOption opt_no_single({APP_NOSINGLE_INSTANCE_SHORT, APP_NOSINGLE_INSTANCE},
                                   QSL("Forces the application to start completely new instance. "
                                       "This is particularly useful when combined with '-%1' option.").arg(opt_config.names().at(0)));
  QCommandLineOption opt_native_dlgs(APP_NAT_DLGS_CONFIG, QSL("Forces usage of native open/save dialogs."));

  m_cmdParser.addOption(opt_config);
  m_cmdParser.addOption(opt_no_single);
  m_cmdParser.addOption(opt_quit);
  m_cmdParser.addOption(opt_already_running);
  m_cmdParser.addOption(opt_native_dlgs);
  m_cmdParser.addPositionalArgument(QSL("files"), QSL("Text files to be opened."), QSL("file-1 ... file-n"));
  m_cmdParser.addHelpOption();
  m_cmdParser.addVersionOption();
  m_cmdParser.setApplicationDescription(QSL("%1 is simple cross-platform text editor built on top of Qt.").arg(APP_NAME));

  m_cmdParser.process(*this);

  auto aa = m_cmdParser.optionNames();

  if (m_cmdParser.isSet(opt_quit)) {
    qWarning().noquote().nospace() << QSL("Application termination is requested by user.");
    qApp->exit(EXIT_SUCCESS);
  }
}

void Application::processExecutionMessage(const QString& message) {
  qDebug().noquote().nospace() << QSL("Received '")
                               << message
                               << QSL("' execution message from another application instance.");
  QStringList messages = message.split(ARGUMENTS_LIST_SEPARATOR);

  messages.prepend(qApp->applicationFilePath());
  m_cmdParser.process(messages);

  if (m_cmdParser.isSet(APP_QUIT_INSTANCE_SHORT)) {
    quitApplication();
  }
  else {
    if (m_cmdParser.isSet(APP_IS_RUNNING_SHORT)) {
      qApp->textApplication()->loadFilesFromArgs(m_cmdParser.positionalArguments());
      qApp->mainForm()->display();
    }
  }
}

void Application::showGuiMessage(const QString& message, QMessageBox::Icon message_type,
                                 const QUrl& url, QObject* target) {
  Q_UNUSED(message_type)

  if (SystemTrayIcon::isSystemTrayActivated()) {
    trayIcon()->showMessage(APP_NAME, message, QSystemTrayIcon::MessageIcon(message_type),
                            TRAY_ICON_BUBBLE_TIMEOUT, target);
  }
  else {
    m_textApplication->outputSidebar()->displayOutput(OutputSource::Application, message,
                                                      message_type, url, target);
  }
}

Application* Application::instance() {
  return static_cast<Application*>(QCoreApplication::instance());
}

void Application::onCommitData(QSessionManager& manager) {
  qDebug().noquote() << QSL("OS asked application to commit its data.");
  manager.setRestartHint(QSessionManager::RestartNever);

  // Now we need to ask user to save any unsaved documents.
  if (manager.allowsInteraction()) {
    bool ok;
    emit dataSaveRequested(&ok);

    manager.release();

    if (!ok) {
      // Not all documents are saved.
      manager.cancel();
    }
  }
}

void Application::onSaveState(QSessionManager& manager) {
  qDebug().noquote() << QSL("OS asked application to save its state.");
  manager.setRestartHint(QSessionManager::RestartNever);
}

void Application::quitApplication() {
  if (m_mainForm != nullptr) {
    m_isQuitting = true;
    bool closed = m_mainForm->close();

    if (closed) {
      quit();
    }
    else {
      m_isQuitting = false;
      m_shouldRestart = false;
    }
  }
}

void Application::onAboutToQuit() {
  eliminateFirstRun();
  eliminateFirstRun(APP_VERSION);
  processEvents();

  qDebug().noquote() << QSL("Cleaning up resources and saving application state.");

#if defined(Q_OS_WIN)
  system()->removeTrolltechJunkRegistryKeys();
#endif

  if (mainForm() != nullptr) {
    mainForm()->saveSize();
  }

  // Now, we can check if application should just quit or restart itself.
  if (m_shouldRestart) {
    finish();
    qDebug().noquote() << QSL("Killing local peer connection to allow another instance to start.");

    if (QProcess::startDetached(QString("\"") + QDir::toNativeSeparators(applicationFilePath()) + QString("\""))) {
      qDebug().noquote() << QSL("New application instance was started.");
    }
    else {
      qWarning().noquote() << QSL("New application instance was not started successfully.");
    }
  }
}

void Application::restart() {
  m_shouldRestart = true;
  quitApplication();
}
