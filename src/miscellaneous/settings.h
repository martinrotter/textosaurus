// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>

#include "definitions/definitions.h"

#include "miscellaneous/settingsproperties.h"

#include <Qsci/qsciscintilla.h>

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
  KEY EolMode;

  VALUE(int) EolModeDef;

  KEY WordWrap;

  VALUE(bool) WordWrapDef;

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
  KEY ID;
  KEY CustomExternalBrowserEnabled;

  VALUE(bool) CustomExternalBrowserEnabledDef;

  KEY CustomExternalBrowserExecutable;

  VALUE(QString) CustomExternalBrowserExecutableDef;

  KEY CustomExternalBrowserArguments;

  VALUE(char*) CustomExternalBrowserArgumentsDef;

  KEY CustomExternalEmailEnabled;

  VALUE(bool) CustomExternalEmailEnabledDef;

  KEY CustomExternalEmailExecutable;

  VALUE(QString) CustomExternalEmailExecutableDef;

  KEY CustomExternalEmailArguments;

  VALUE(char*) CustomExternalEmailArgumentsDef;
}

// General.
namespace General {
  KEY ID;
  KEY UpdateOnStartup;

  VALUE(bool) UpdateOnStartupDef;

  KEY RemoveTrolltechJunk;

  VALUE(bool) RemoveTrolltechJunkDef;

  KEY FirstRun;

  VALUE(bool) FirstRunDef;

  KEY Language;

  VALUE(QString) LanguageDef;
}

// Proxy.
namespace Proxy {
  KEY ID;
  KEY Type;

  VALUE(QNetworkProxy::ProxyType) TypeDef;

  KEY Host;

  VALUE(QString) HostDef;

  KEY Username;

  VALUE(QString) UsernameDef;

  KEY Password;

  VALUE(QString) PasswordDef;

  KEY Port;

  VALUE(int) PortDef;
}

// Keyboard.
namespace Keyboard {
  KEY ID;
}

class Settings : public QSettings {
  Q_OBJECT

  public:

    // Destructor.
    virtual ~Settings();

    // Type of used settings.
    inline SettingsProperties::SettingsType type() const {
      return m_initializationStatus;
    }

    // Getters/setters for settings values.
    inline QVariant value(const QString& section, const QString& key, const QVariant& default_value = QVariant()) const {
      return QSettings::value(QString(QSL("%1/%2")).arg(section, key), default_value);
    }

    inline void setValue(const QString& section, const QString& key, const QVariant& value) {
      QSettings::setValue(QString(QSL("%1/%2")).arg(section, key), value);
    }

    inline void setValue(const QString& key, const QVariant& value) {
      QSettings::setValue(key, value);
    }

    inline bool contains(const QString& section, const QString& key) const {
      return QSettings::contains(QString(QSL("%1/%2")).arg(section, key));
    }

    inline void remove(const QString& section, const QString& key) {
      QSettings::remove(QString(QSL("%1/%2")).arg(section, key));
    }

    // Returns the path which contains the settings.
    QString pathName() const;

    // Synchronizes settings.
    QSettings::Status checkSettings();

    // Creates settings file in correct location.
    static Settings* setupSettings(QObject* parent);

    // Returns properties of the actual application-wide settings.
    static SettingsProperties determineProperties();

  private:

    // Constructor.
    explicit Settings(const QString& file_name, Format format, const SettingsProperties::SettingsType& type, QObject* parent = nullptr);

    SettingsProperties::SettingsType m_initializationStatus;
};

#endif // SETTINGS_H
