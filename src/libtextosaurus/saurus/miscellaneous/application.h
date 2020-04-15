// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifndef APPLICATION_H
#define APPLICATION_H

#include "3rd-party/qtsingleapplication/qtsingleapplication.h"

#include "common/miscellaneous/iofactory.h"
#include "common/miscellaneous/localization.h"
#include "common/miscellaneous/settings.h"
#include "common/miscellaneous/systemfactory.h"
#include "definitions/definitions.h"

#include <QCommandLineParser>
#include <QList>
#include <QMessageBox>

#include <functional>

#if defined(qApp)
#undef qApp
#endif

// Define new qApp macro. Yeaaaaah.
#define qApp (Application::instance())

class FormMain;
class IconFactory;
class QAction;
class WebFactory;
class SystemTrayIcon;
class TextApplication;
class TextEditor;

class TEXTOSAURUS_DLLSPEC Application : public QtSingleApplication {
  Q_OBJECT

  public:
    explicit Application(const QString& id, int& argc, char** argv);

    void activateQtSingleMsgProcessing();

    bool isRunning();
    bool forcesNewInstance() const;
    bool forcesNativeDialogs() const;

    // Globally accessible actions.
    QList<QAction*> userActions();

    // Check whether this application starts for the first time (ever).
    bool isFirstRun() const;

    // Check whether GIVEN VERSION of the application starts for the first time.
    bool isFirstRun(const QString& version) const;

    void setTextApplication(TextApplication* text_application);
    TextApplication* textApplication() const;
    WebFactory* web() const;
    SystemFactory* system();
    Localization* localization();
    IconFactory* icons();
    Settings* settings() const;
    FormMain* mainForm();
    QWidget* mainFormWidget();
    SystemTrayIcon* trayIcon();
    QCommandLineParser* cmdParser();

    QString homeFolder();
    QString configFolder();

    // Returns the base folder to which store user data, the "data" folder.
    // NOTE: Use this to get correct path under which store user data.
    QString userDataFolder();

    void setMainForm(FormMain* main_form);

    // Displays given simple message in tray icon bubble or OSD
    // or in message box if tray icon is disabled.
    void showGuiMessage(const QString& message, QMessageBox::Icon message_type = QMessageBox::Icon::NoIcon,
                        const QUrl& url = QUrl(), QObject* target = nullptr);

    // Returns pointer to "GOD" application singleton.
    static Application* instance();

    void showTrayIcon();
    void deleteTrayIcon();

    bool isQuitting() const;

    void parseCmdArguments();

  public slots:
    void quitApplication();
    void restart();

    // Processes incoming message from another application instance.
    void processExecutionMessage(const QString& message);

  signals:
    void dataSaveRequested(bool* ok);

  private slots:
    void onCommitData(QSessionManager& manager);
    void onSaveState(QSessionManager& manager);
    void onAboutToQuit();

  private:
    void eliminateFirstRun();
    void eliminateFirstRun(const QString& version);

    // These return user ready folders.
    QString userDataAppFolder();
    QString userDataHomeFolder();
    QString userDataForcedFolder();

  private:
    QCommandLineParser m_cmdParser;
    Settings* m_settings;
    TextApplication* m_textApplication;
    FormMain* m_mainForm;
    WebFactory* m_webFactory;
    SystemFactory* m_system;
    Localization* m_localization;
    IconFactory* m_icons;
    SystemTrayIcon* m_trayIcon;
    bool m_shouldRestart;
    bool m_isQuitting;
};

#endif // APPLICATION_H
