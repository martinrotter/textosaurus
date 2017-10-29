// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef APPLICATION_H
#define APPLICATION_H

#include "qtsingleapplication/qtsingleapplication.h"

#include "definitions/definitions.h"
#include "miscellaneous/iofactory.h"
#include "miscellaneous/localization.h"
#include "miscellaneous/settings.h"
#include "miscellaneous/systemfactory.h"

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
class TextApplication;

class Application : public QtSingleApplication {
  Q_OBJECT

  public:

    // Constructors and destructors.
    explicit Application(const QString& id, int& argc, char** argv);
    virtual ~Application();

    // Globally accessible actions.
    QList<QAction*> userActions();

    // Check whether this application starts for the first time (ever).
    bool isFirstRun() const;

    // Check whether GIVEN VERSION of the application starts for the first time.
    bool isFirstRun(const QString& version) const;

    TextApplication* textApplication() const;
    WebFactory* web() const;
    SystemFactory* system();
    Localization* localization();
    IconFactory* icons();
    Settings* settings() const;
    FormMain* mainForm();
    QWidget* mainFormWidget();

    QString tempFolder();
    QString documentsFolder();
    QString homeFolder();
    QString configFolder();

    // These return user ready folders.
    QString userDataAppFolder();
    QString userDataHomeFolder();

    // Returns the base folder to which store user data, the "data" folder.
    // NOTE: Use this to get correct path under which store user data.
    QString userDataFolder();

    void setMainForm(FormMain* main_form);

    // Displays given simple message in tray icon bubble or OSD
    // or in message box if tray icon is disabled.
    void showGuiMessage(const QString& title, const QString& message, QMessageBox::Icon message_type,
                        QWidget* parent = nullptr, bool show_at_least_msgbox = false,
                        std::function<void()> functor = nullptr);

    // Returns pointer to "GOD" application singleton.
    static Application* instance();

  public slots:

    // Restarts the application.
    void restart();

    // Processes incoming message from another RSS Guard instance.
    void processExecutionMessage(const QString& message);

  signals:
    void dataSaveRequested(bool& ok);

  private slots:

    // Last-minute reactors.
    void onCommitData(QSessionManager& manager);
    void onSaveState(QSessionManager& manager);
    void onAboutToQuit();

  private:
    void eliminateFirstRun();
    void eliminateFirstRun(const QString& version);

    QList<QAction*> m_userActions;
    TextApplication* m_textApplication;
    FormMain* m_mainForm;
    Settings* m_settings;
    WebFactory* m_webFactory;
    SystemFactory* m_system;
    Localization* m_localization;
    IconFactory* m_icons;
    bool m_shouldRestart;
};

#endif // APPLICATION_H
