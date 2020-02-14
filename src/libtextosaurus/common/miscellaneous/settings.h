// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>

#include "definitions/definitions.h"

#include "common/miscellaneous/iofactory.h"
#include "common/miscellaneous/settingsproperties.h"

// *INDENT-OFF*
#include PATH(SCINTILLA_DIR,include/Scintilla.h)

// *INDENT-ON*

#include <QByteArray>
#include <QColor>
#include <QDateTime>
#include <QLocale>
#include <QStandardPaths>
#include <QStringList>

#define GROUP(x) x::ID
#define SETTING(x) x, x ## Def
#define DEFAULT_VALUE(x) x ## Def

namespace DisabledPlugins {
  constexpr auto ID = "disabled_plugins";
}

namespace Editor {
  constexpr auto ID = "editor";
  constexpr auto LogTimestampFormat = "log_timestamp_format";
  constexpr auto LogTimestampFormatDef = "yyyy-MM-dd HH:mm:ss";
  constexpr auto DateTimeTimestampFormat = "dt_timestamp_format";
  constexpr auto DateTimeTimestampFormatDef = "yyyy-MM-dd HH:mm:ss";
  constexpr auto TabSize = "tab_size";
  constexpr auto TabSizeDef = 2;
  constexpr auto ColorTheme = "color_theme";
  constexpr auto EdgeLongLine = "edge_long_line";
  constexpr auto EdgeLongLineDef = false;
  constexpr auto EdgeLongLineColumn = "edge_long_line_column";
  constexpr auto EdgeLongLineColumnDef = 60;
  constexpr auto IndentSize = "indent_size";
  constexpr auto IndentSizeDef = 2;
  constexpr auto IndentWithTabs = "indent_with_tabs";
  constexpr auto IndentWithTabsDef = false;
  constexpr auto LineSpacing = "line_spacing";
  constexpr auto LineSpacingDef = 0;
  constexpr auto ReloadModified = "reload_modified_files";
  constexpr auto ReloadModifiedDef = false;
  constexpr auto EolMode = "eol_mode";

#if defined(Q_OS_WIN)
  constexpr auto EolModeDef = SC_EOL_CRLF;
#elif defined(Q_OS_MAC)
  constexpr auto EolModeDef = SC_EOL_CR;
#else
  constexpr auto EolModeDef = SC_EOL_LF;
#endif

  constexpr auto FontMain = "main_font";
  constexpr auto WordWrap = "word_wrap";
  constexpr auto WordWrapDef = true;
  constexpr auto LineNumbers = "line_numbers";
  constexpr auto LineNumbersDef = false;
  constexpr auto ContextAwareHighlights = "context_aware_highlights";
  constexpr auto ContextAwareHighlightsDef = false;
  constexpr auto CodeFolding = "code_folding";
  constexpr auto CodeFoldingDef = false;
  constexpr auto AutoIndent = "auto_indent";
  constexpr auto AutoIndentDef = true;
  constexpr auto RecentFiles = "recent_files";
  constexpr auto ViewEols = "view_eols";
  constexpr auto ViewEolsDef = false;
  constexpr auto ViewWhitespaces = "view_whitespaces";
  constexpr auto ViewWhitespacesDef = false;
  constexpr auto DefaultLoadSaveDirectory = "load_save_directory";
  const QString DefaultLoadSaveDirectoryDef = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
}

namespace GUI {
  constexpr auto ID = "gui";
  constexpr auto ToolbarStyle = "toolbar_style";
  constexpr Qt::ToolButtonStyle ToolbarStyleDef = Qt::ToolButtonIconOnly;
  constexpr auto ToolbarActions = "toolbar";
  constexpr auto ToolbarActionsDef = "m_actionFileNew,m_actionFileOpen,m_actionFileSave,separator,"
                                     "m_actionEditBack,m_actionEditForward,spacer,m_actionTabsCloseAllUnmodified";
  constexpr auto MainWindowGeometry = "main_window_geometry";
  constexpr auto IsMainWindowMaximizedBeforeFullscreen = "is_window_maximized_before_fullscreen";
  constexpr auto IsMainWindowMaximizedBeforeFullscreenDef = false;
  constexpr auto MainWindowState = "main_window_state";
  constexpr auto ToolbarsVisible = "enable_toolbars";
  constexpr auto ToolbarsVisibleDef = true;
  constexpr auto UseTrayIcon = "use_tray_icon";
  constexpr auto UseTrayIconDef = false;
  constexpr auto EnableNotifications = "enable_tray_notifications";
  constexpr auto EnableNotificationsDef = true;
  constexpr auto HideMainWindowWhenMinimized = "hide_main_window_when_minimized";
  constexpr auto HideMainWindowWhenMinimizedDef = true;
  constexpr auto HideMainWindowWhenClosed = "hide_main_window_when_closed";
  constexpr auto HideMainWindowWhenClosedDef = true;
  constexpr auto StatusBarVisible = "enable_statusbar";
  constexpr auto StatusBarVisibleDef = true;
  constexpr auto StayOnTop = "stay_on_top";
  constexpr auto StayOnTopDef = false;
  constexpr auto TabCloseMiddleClick = "tab_close_mid_button";
  constexpr auto TabCloseMiddleClickDef = true;
  constexpr auto TabCloseDoubleClick = "tab_close_double_button";
  constexpr auto TabCloseDoubleClickDef = true;
  constexpr auto TabNewDoubleClick = "tab_new_double_button";
  constexpr auto TabNewDoubleClickDef = true;
  constexpr auto IconTheme = "icon_theme_name";

#if defined(Q_OS_LINUX)
  constexpr auto IconThemeDef = APP_NO_THEME;
#else
  constexpr auto IconThemeDef = APP_THEME_DEFAULT;
#endif

  const QString Style = QString("style_%1").arg(OS_ID_LOW);
  constexpr auto StyleDef = APP_STYLE_DEFAULT;
}

namespace Browser {
  constexpr auto ID = "browser";
  constexpr auto CustomExternalBrowserEnabled = "custom_external_browser";
  constexpr auto CustomExternalBrowserEnabledDef = false;
  constexpr auto CustomExternalBrowserExecutable = "external_browser_executable";
  constexpr auto CustomExternalBrowserExecutableDef = "";
  constexpr auto CustomExternalBrowserArguments = "external_browser_arguments";
  constexpr auto CustomExternalBrowserArgumentsDef = "%1";
  constexpr auto CustomExternalEmailEnabled = "custom_external_email";
  constexpr auto CustomExternalEmailEnabledDef = false;
  constexpr auto CustomExternalEmailExecutable = "external_email_executable";
  constexpr auto CustomExternalEmailArguments = "external_email_arguments";
}

namespace ToolsPromptValues {
  constexpr auto ID = "external_tools_prompts";
}

namespace StoredMacros {
  constexpr auto ID = "macros";
}

namespace General {
  constexpr auto ID = "main";
  constexpr auto RemoveTrolltechJunk = "remove_trolltech_junk";
  constexpr auto RemoveTrolltechJunkDef = false;
  constexpr auto FirstRun = "first_run";
  constexpr auto FirstRunDef = true;
  constexpr auto EncryptionCipher = "enc_cipher";
  constexpr auto EncryptionCipherDef = "-aes-256-cbc";
  constexpr auto EncryptionUsePbkdf2 = "enc_pbkdf2";
  constexpr auto EncryptionUsePbkdf2Def = false;
  constexpr auto RestoreSession = "restore_session";
  constexpr auto RestoreSessionDef = false;
  const QString RestoreSessionIndex = QString("restore_session_index_%1").arg(OS_ID_LOW);
  constexpr auto RestoreSessionIndexDef = -1;
  const QString RestoreSessionFiles = QString("restore_session_%1").arg(OS_ID_LOW);
  const QStringList RestoreSessionFilesDef = QStringList();
  constexpr auto Language = "language";
  const auto LanguageDef = QLocale::system().name();
}

namespace Proxy {
  constexpr auto ID = "proxy";
  constexpr auto Type = "proxy_type";
  constexpr auto Host = "host";
  constexpr auto Username = "username";
  constexpr auto Password = "password";
  constexpr auto Port = "port";
  constexpr auto PortDef = 80;
}

namespace Keyboard {
  constexpr auto ID = "keyboard";
}

class TEXTOSAURUS_DLLSPEC Settings : public QSettings {
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
    QString baseFolder() const;

    // Synchronizes settings.
    QSettings::Status checkSettings();

    void printSettingsInfo();

    // Creates settings file in correct location.
    static Settings* setupSettings(QObject* parent, const QString& app_path, const QString& user_path, const QString& forced_path);

    // Returns properties of the actual application-wide settings.
    static SettingsProperties determineProperties(const QString& app_path, const QString& user_path, const QString& forced_path);

  private:
    explicit Settings(const QString& file_name, Format format, SettingsType type, QObject* parent = nullptr);

    SettingsType m_settingsType;
};

inline SettingsType Settings::type() const {
  return m_settingsType;
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
