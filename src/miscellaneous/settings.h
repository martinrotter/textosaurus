// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>

#include "definitions/definitions.h"

#include "miscellaneous/settingsproperties.h"

#include <QByteArray>
#include <QColor>
#include <QDateTime>
#include <QNetworkProxy>
#include <QStringList>

#define KEY extern const char*
#define DKEY const char*
#define VALUE(x) extern const x
#define NON_CONST_VALUE(x) extern x
#define DVALUE(x) const x
#define NON_CONST_DVALUE(x) x
#define SETTING(x) x, x ## Def
#define DEFAULT_VALUE(x) x ## Def
#define GROUP(x) x::ID

// Editor.
namespace Editor {
  KEY ID;
  KEY TabSize;

  VALUE(int) TabSizeDef;

  KEY IndentSize;

  VALUE(int) IndentSizeDef;

  KEY IndentWithTabs;

  VALUE(bool) IndentWithTabsDef;

  KEY LineSpacing;

  VALUE(int) LineSpacingDef;

  KEY EolMode;

  VALUE(int) EolModeDef;

  KEY FontMain;
  KEY RecentFiles;

  VALUE(QStringList) RecentFilesDef;

  KEY WordWrap;

  VALUE(bool) WordWrapDef;

  KEY LineNumbers;

  VALUE(bool) LineNumbersDef;

  KEY ViewEols;

  VALUE(bool) ViewEolsDef;

  KEY ViewWhitespaces;

  VALUE(bool) ViewWhitespacesDef;

  KEY DefaultLoadSaveDirectory;

  VALUE(QString) DefaultLoadSaveDirectoryDef;
}

// GUI.
namespace GUI {
  KEY ID;
  KEY ToolbarStyle;

  VALUE(Qt::ToolButtonStyle) ToolbarStyleDef;

  KEY ToolbarActions;

  VALUE(char*) ToolbarActionsDef;

  KEY MainWindowInitialSize;
  KEY MainWindowInitialPosition;
  KEY IsMainWindowMaximizedBeforeFullscreen;

  VALUE(bool) IsMainWindowMaximizedBeforeFullscreenDef;

  KEY MainWindowStartsFullscreen;

  VALUE(bool) MainWindowStartsFullscreenDef;

  KEY MainWindowStartsMaximized;

  VALUE(bool) MainWindowStartsMaximizedDef;

  KEY MainMenuVisible;

  VALUE(bool) MainMenuVisibleDef;

  KEY ToolbarsVisible;

  VALUE(bool) ToolbarsVisibleDef;

  KEY StatusBarVisible;

  VALUE(bool) StatusBarVisibleDef;

  KEY EnableNotifications;

  VALUE(bool) EnableNotificationsDef;

  KEY TabCloseMiddleClick;

  VALUE(bool) TabCloseMiddleClickDef;

  KEY TabCloseDoubleClick;

  VALUE(bool) TabCloseDoubleClickDef;

  KEY TabNewDoubleClick;

  VALUE(bool) TabNewDoubleClickDef;

  KEY IconTheme;

  VALUE(char*) IconThemeDef;

  KEY Style;

  VALUE(char*) StyleDef;
}

// Web browser.
namespace Browser {
  constexpr auto ID = "browser";
  constexpr auto CustomExternalBrowserEnabled = "custom_external_browser";
  constexpr auto CustomExternalBrowserEnabledDef = false;
  constexpr auto CustomExternalBrowserExecutable = "external_browser_executable";
  const QString CustomExternalBrowserExecutableDef = QString();
  constexpr auto CustomExternalBrowserArguments = "external_browser_arguments";
  const QString CustomExternalBrowserArgumentsDef = "%1";
  constexpr auto CustomExternalEmailEnabled = "custom_external_email";
  constexpr auto CustomExternalEmailEnabledDef = false;
  constexpr auto CustomExternalEmailExecutable = "external_email_executable";
  const QString CustomExternalEmailExecutableDef = QString();
  constexpr auto CustomExternalEmailArguments = "external_email_arguments";
  const QString CustomExternalEmailArgumentsDef = QString();
}

// General.
namespace General {
  constexpr auto ID = "main";
  constexpr auto RemoveTrolltechJunk = "remove_trolltech_junk";
  constexpr auto RemoveTrolltechJunkDef = false;
  constexpr auto FirstRun = "first_run";
  constexpr auto FirstRunDef = true;
  constexpr auto Language = "language";
  const auto LanguageDef = QLocale::system().name();
}

// Proxy.
namespace Proxy {
  constexpr auto ID = "proxy";
  constexpr auto Type = "proxy_type";
  constexpr QNetworkProxy::ProxyType TypeDef = QNetworkProxy::NoProxy;
  constexpr auto Host = "host";
  constexpr auto HostDef = "";
  constexpr auto Username = "username";
  constexpr auto UsernameDef = "";
  constexpr auto Password = "password";
  constexpr auto PasswordDef = "";
  constexpr auto Port = "port";
  constexpr auto PortDef = 80;
}

// Keyboard.
namespace Keyboard {
  constexpr auto ID = "keyboard";
}

class Settings : public QSettings {
  Q_OBJECT

  public:
    virtual ~Settings() = default;

    // Type of used settings.
    SettingsType type() const;

    QVariant value(const QString& section, const QString& key, const QVariant& default_value = QVariant()) const;

    void setValue(const QString& section, const QString& key, const QVariant& value);
    void setValue(const QString& key, const QVariant& value);
    bool contains(const QString& section, const QString& key) const;
    void remove(const QString& section, const QString& key);

    // Returns the path which contains the settings.
    QString pathName() const;

    // Synchronizes settings.
    QSettings::Status checkSettings();

    // Creates settings file in correct location.
    static Settings* setupSettings(QObject* parent);

    // Returns properties of the actual application-wide settings.
    static SettingsProperties determineProperties();

  private:
    explicit Settings(const QString& file_name, Format format, SettingsType type, QObject* parent = nullptr);

    SettingsType m_initializationStatus;
};

inline SettingsType Settings::type() const {
  return m_initializationStatus;
}

inline QVariant Settings::value(const QString& section, const QString& key, const QVariant& default_value) const {
  return QSettings::value(QString(QSL("%1/%2")).arg(section, key), default_value);
}

inline void Settings::setValue(const QString& section, const QString& key, const QVariant& value) {
  QSettings::setValue(QString(QSL("%1/%2")).arg(section, key), value);
}

inline void Settings::setValue(const QString& key, const QVariant& value) {
  QSettings::setValue(key, value);
}

inline bool Settings::contains(const QString& section, const QString& key) const {
  return QSettings::contains(QString(QSL("%1/%2")).arg(section, key));
}

inline void Settings::remove(const QString& section, const QString& key) {
  QSettings::remove(QString(QSL("%1/%2")).arg(section, key));
}

#endif // SETTINGS_H
