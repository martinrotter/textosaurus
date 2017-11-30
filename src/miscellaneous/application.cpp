// For license of this file, see <project-root-folder>/LICENSE.md.

#include "miscellaneous/application.h"

#include "exceptions/applicationexception.h"
#include "gui/dialogs/formmain.h"
#include "gui/messagebox.h"
#include "gui/statusbar.h"
#include "miscellaneous/iconfactory.h"
#include "miscellaneous/iofactory.h"
#include "miscellaneous/textapplication.h"
#include "network-web/webfactory.h"

#include <QProcess>
#include <QSessionManager>

Application::Application(const QString& id, int& argc, char** argv)
  : QtSingleApplication(id, argc, argv),
  m_userActions(QList<QAction*>()), m_textApplication(new TextApplication(this)), m_mainForm(nullptr),
  m_settings(Settings::setupSettings(this)), m_webFactory(new WebFactory(this)),
  m_system(new SystemFactory(this)),
  m_localization(new Localization(this)), m_icons(new IconFactory(this)),
  m_shouldRestart(false) {
  connect(this, &Application::aboutToQuit, this, &Application::onAboutToQuit);
  connect(this, &Application::commitDataRequest, this, &Application::onCommitData);
  connect(this, &Application::saveStateRequest, this, &Application::onSaveState);

  setQuitOnLastWindowClosed(true);
  setStyleSheet(QSL("QStatusBar::item { border: none; } "
                    "QSplitter::handle:horizontal, QSplitter::handle:vertical { width: 1px; }"));
}

Application::~Application() {
  qDebug("Destroying Application instance.");
}

QList<QAction*> Application::userActions() {
  if (m_mainForm != nullptr && m_userActions.isEmpty()) {
    m_userActions = m_mainForm->allActions();
  }

  return m_userActions;
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

void Application::setMainForm(FormMain* main_form) {
  m_mainForm = main_form;
}

QString Application::configFolder() {
  return IOFactory::getSystemFolder(QStandardPaths::GenericConfigLocation);
}

QString Application::userDataAppFolder() {
  // In "app" folder, we would like to separate all user data into own subfolder,
  // therefore stick to "data" folder in this mode.
  return applicationDirPath() + QDir::separator() + QSL("data");
}

QString Application::userDataFolder() {
  if (settings()->type() == SettingsProperties::Portable) {
    return userDataAppFolder();
  }
  else {
    return userDataHomeFolder();
  }
}

QString Application::userDataHomeFolder() {
#if defined (Q_OS_ANDROID)
  return IOFactory::getSystemFolder(QStandardPaths::GenericDataLocation) + QDir::separator() + QSL(APP_NAME);
#else
  return configFolder() + QDir::separator() + QSL(APP_NAME);
#endif
}

QString Application::tempFolder() {
  return IOFactory::getSystemFolder(QStandardPaths::TempLocation);
}

QString Application::documentsFolder() {
  return IOFactory::getSystemFolder(QStandardPaths::DocumentsLocation);
}

QString Application::homeFolder() {
#if defined (Q_OS_ANDROID)
  return IOFactory::getSystemFolder(QStandardPaths::GenericDataLocation);
#else
  return IOFactory::getSystemFolder(QStandardPaths::HomeLocation);
#endif
}

void Application::processExecutionMessage(const QString& message) {
  qDebug("Received '%s' execution message from another application instance.", qPrintable(message));
  const QStringList messages = message.split(ARGUMENTS_LIST_SEPARATOR);

  if (messages.contains(APP_QUIT_INSTANCE)) {
    quitApplication();
  }
  else {
    if (messages.contains(QSL(APP_IS_RUNNING))) {
      qApp->textApplication()->loadFilesFromArgs(messages);
      qApp->mainForm()->display();
    }
  }
}

void Application::showGuiMessage(const QString& message, QMessageBox::Icon message_type) {
  Q_UNUSED(message_type)

  m_textApplication->outputWindow()->displayOutput(OutputSource::Application, message, message_type);
}

Application* Application::instance() {
  return static_cast<Application*>(QCoreApplication::instance());
}

void Application::quitApplication() {
  if (m_mainForm != nullptr) {
    m_mainForm->close();
  }
  else {
    Application::quit();
  }
}

void Application::onCommitData(QSessionManager& manager) {
  qDebug("OS asked application to commit its data.");
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
  qDebug("OS asked application to save its state.");
  manager.setRestartHint(QSessionManager::RestartNever);
}

void Application::onAboutToQuit() {
  eliminateFirstRun();
  eliminateFirstRun(APP_VERSION);
  processEvents();

  qDebug("Cleaning up resources and saving application state.");

#if defined(Q_OS_WIN)
  system()->removeTrolltechJunkRegistryKeys();
#endif

  if (mainForm() != nullptr) {
    mainForm()->saveSize();
  }

  // Now, we can check if application should just quit or restart itself.
  if (m_shouldRestart) {
    finish();
    qDebug("Killing local peer connection to allow another instance to start.");

    if (QProcess::startDetached(QString("\"") + QDir::toNativeSeparators(applicationFilePath()) + QString("\""))) {
      qDebug("New application instance was started.");
    }
    else {
      qWarning("New application instance was not started successfully.");
    }
  }
}

void Application::restart() {
  m_shouldRestart = true;

  quitApplication();
}
