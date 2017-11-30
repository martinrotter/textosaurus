// For license of this file, see <project-root-folder>/LICENSE.md.

#include "miscellaneous/settings.h"

#include "miscellaneous/application.h"
#include "miscellaneous/iofactory.h"

#include "scintilla/include/Scintilla.h"

#include <QDebug>
#include <QDir>
#include <QFontDatabase>
#include <QLocale>
#include <QPointer>

// Editor.
DKEY Editor::ID = "editor";
DKEY Editor::LineSpacing = "line_spacing";

DVALUE(int) Editor::LineSpacingDef = 0;

DKEY Editor::EolMode = "eol_mode";

#if defined(Q_OS_WIN)
DVALUE(int) Editor::EolModeDef = SC_EOL_CRLF;
#elif defined(Q_OS_MAC)
DVALUE(int) Editor::EolModeDef = SC_EOL_CR;
#else
DVALUE(int) Editor::EolModeDef = SC_EOL_LF;
#endif

DKEY Editor::FontMain = "main_font";
DKEY Editor::WordWrap = "word_wrap";

DVALUE(bool) Editor::WordWrapDef = true;

DKEY Editor::LineNumbers = "line_numbers";

DVALUE(bool) Editor::LineNumbersDef = false;

DKEY Editor::RecentFiles = "recent_files";

DVALUE(QStringList) Editor::RecentFilesDef = QStringList();

DKEY Editor::ViewEols = "view_eols";

DVALUE(bool) Editor::ViewEolsDef = false;

DKEY Editor::ViewWhitespaces = "view_whitespaces";

DVALUE(bool) Editor::ViewWhitespacesDef = false;

DKEY Editor::DefaultLoadSaveDirectory = "load_save_directory";

DVALUE(QString) Editor::DefaultLoadSaveDirectoryDef = IOFactory::getSystemFolder(QStandardPaths::DocumentsLocation);

// GUI.
DKEY GUI::ID = "gui";
DKEY GUI::ToolbarStyle = "toolbar_style";

DVALUE(Qt::ToolButtonStyle) GUI::ToolbarStyleDef = Qt::ToolButtonIconOnly;

DKEY GUI::ToolbarActions = "toolbar";

DVALUE(char*) GUI::ToolbarActionsDef =
  "m_actionFileNew,m_actionFileOpen,m_actionFileSave,separator,m_actionEditBack,m_actionEditForward,separator,spacer,m_actionTabsCloseAllUnmodified";

DKEY GUI::MainWindowInitialSize = "window_size";
DKEY GUI::MainWindowInitialPosition = "window_position";
DKEY GUI::IsMainWindowMaximizedBeforeFullscreen = "is_window_maximized_before_fullscreen";

DVALUE(bool) GUI::IsMainWindowMaximizedBeforeFullscreenDef = false;

DKEY GUI::MainWindowStartsFullscreen = "start_in_fullscreen";

DVALUE(bool) GUI::MainWindowStartsFullscreenDef = false;

DKEY GUI::MainWindowStartsMaximized = "window_is_maximized";

DVALUE(bool) GUI::MainWindowStartsMaximizedDef = false;

DKEY GUI::MainMenuVisible = "main_menu_visible";

DVALUE(bool) GUI::MainMenuVisibleDef = true;

DKEY GUI::ToolbarsVisible = "enable_toolbars";

DVALUE(bool) GUI::ToolbarsVisibleDef = true;

DKEY GUI::StatusBarVisible = "enable_status_bar";

DVALUE(bool) GUI::StatusBarVisibleDef = true;

DKEY GUI::EnableNotifications = "enable_notifications";

DVALUE(bool) GUI::EnableNotificationsDef = true;

DKEY GUI::TabCloseMiddleClick = "tab_close_mid_button";

DVALUE(bool) GUI::TabCloseMiddleClickDef = true;

DKEY GUI::TabCloseDoubleClick = "tab_close_double_button";

DVALUE(bool) GUI::TabCloseDoubleClickDef = true;

DKEY GUI::TabNewDoubleClick = "tab_new_double_button";

DVALUE(bool) GUI::TabNewDoubleClickDef = true;

DKEY GUI::IconTheme = "icon_theme_name";

#if defined(Q_OS_LINUX)
DVALUE(char*) GUI::IconThemeDef = APP_NO_THEME;
#else
DVALUE(char*) GUI::IconThemeDef = APP_THEME_DEFAULT;
#endif

DKEY GUI::Style = "style";

DVALUE(char*) GUI::StyleDef = APP_STYLE_DEFAULT;

// Web browser.
DKEY Browser::ID = "browser";
DKEY Browser::CustomExternalBrowserEnabled = "custom_external_browser";

DVALUE(bool) Browser::CustomExternalBrowserEnabledDef = false;

DKEY Browser::CustomExternalBrowserExecutable = "external_browser_executable";

DVALUE(QString) Browser::CustomExternalBrowserExecutableDef = QString();

DKEY Browser::CustomExternalBrowserArguments = "external_browser_arguments";

DVALUE(char*) Browser::CustomExternalBrowserArgumentsDef = "%1";

DKEY Browser::CustomExternalEmailEnabled = "custom_external_email";

DVALUE(bool) Browser::CustomExternalEmailEnabledDef = false;

DKEY Browser::CustomExternalEmailExecutable = "external_email_executable";

DVALUE(QString) Browser::CustomExternalEmailExecutableDef = QString();

DKEY Browser::CustomExternalEmailArguments = "external_email_arguments";

DVALUE(char*) Browser::CustomExternalEmailArgumentsDef = "";

// General.
DKEY General::ID = "main";
DKEY General::RemoveTrolltechJunk = "remove_trolltech_junk";

DVALUE(bool) General::RemoveTrolltechJunkDef = false;

DKEY General::FirstRun = "first_run";

DVALUE(bool) General::FirstRunDef = true;

DKEY General::Language = "language";

DVALUE(QString) General::LanguageDef = QLocale::system().name();

// Proxy.
DKEY Proxy::ID = "proxy";
DKEY Proxy::Type = "proxy_type";

DVALUE(QNetworkProxy::ProxyType) Proxy::TypeDef = QNetworkProxy::NoProxy;

DKEY Proxy::Host = "host";

DVALUE(QString) Proxy::HostDef = QString();

DKEY Proxy::Username = "username";

DVALUE(QString) Proxy::UsernameDef = QString();

DKEY Proxy::Password = "password";

DVALUE(QString) Proxy::PasswordDef = QString();

DKEY Proxy::Port = "port";

DVALUE(int) Proxy::PortDef = 80;

// Keyboard.
DKEY Keyboard::ID = "keyboard";

Settings::Settings(const QString& file_name, Format format, const SettingsProperties::SettingsType& status, QObject* parent)
  : QSettings(file_name, format, parent), m_initializationStatus(status) {}

Settings::~Settings() {}

QString Settings::pathName() const {
  return QFileInfo(fileName()).absolutePath();
}

QSettings::Status Settings::checkSettings() {
  qDebug("Syncing settings.");
  sync();
  return status();
}

Settings* Settings::setupSettings(QObject* parent) {
  Settings* new_settings;

  // If settings file exists (and is writable) in executable file working directory
  // (in subdirectory APP_CFG_PATH), then use it (portable settings).
  // Otherwise use settings file stored in home path.
  const SettingsProperties properties = determineProperties();

  // Portable settings are available, use them.
  new_settings = new Settings(properties.m_absoluteSettingsFileName, QSettings::IniFormat, properties.m_type, parent);

  // Check if portable settings are available.
  if (properties.m_type == SettingsProperties::Portable) {
    qDebug("Initializing settings in '%s' (portable way).", qPrintable(QDir::toNativeSeparators(properties.m_absoluteSettingsFileName)));
  }
  else {
    qDebug("Initializing settings in '%s' (non-portable way).",
           qPrintable(QDir::toNativeSeparators(properties.m_absoluteSettingsFileName)));
  }

  return new_settings;
}

SettingsProperties Settings::determineProperties() {
  SettingsProperties properties;

  properties.m_settingsSuffix = QDir::separator() + QSL(APP_CFG_PATH) + QDir::separator() + QSL(APP_CFG_FILE);
  const QString app_path = qApp->userDataAppFolder();
  const QString home_path = qApp->userDataHomeFolder();

  // We will use PORTABLE settings only and only if it is available and NON-PORTABLE
  // settings was not initialized before.
#if defined (Q_OS_LINUX) || defined (Q_OS_ANDROID) || defined (Q_OS_MACOS)

  // DO NOT use portable settings for Linux, it is really not used on that platform.
  const bool will_we_use_portable_settings = false;
#else
  const QString exe_path = qApp->applicationDirPath();
  const QString home_path_file = home_path + properties.m_settingsSuffix;
  const bool portable_settings_available = IOFactory::isFolderWritable(exe_path);
  const bool non_portable_settings_exist = QFile::exists(home_path_file);
  const bool will_we_use_portable_settings = portable_settings_available && !non_portable_settings_exist;
#endif

  if (will_we_use_portable_settings) {
    properties.m_type = SettingsProperties::Portable;
    properties.m_baseDirectory = app_path;
  }
  else {
    properties.m_type = SettingsProperties::NonPortable;
    properties.m_baseDirectory = home_path;
  }

  properties.m_absoluteSettingsFileName = properties.m_baseDirectory + properties.m_settingsSuffix;
  return properties;
}
