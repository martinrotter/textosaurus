// For license of this file, see <project-root-folder>/LICENSE.md.

#include "common/miscellaneous/settings.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFontDatabase>
#include <QLocale>
#include <QPointer>

Settings::Settings(const QString& file_name, Format format, SettingsType status, QObject* parent)
  : QSettings(file_name, format, parent), m_initializationStatus(status) {}

QString Settings::pathName() const {
  return QFileInfo(fileName()).absolutePath();
}

QSettings::Status Settings::checkSettings() {
  qDebug("Syncing settings.");
  sync();
  return status();
}

Settings* Settings::setupSettings(QObject* parent, const QString& app_path, const QString& user_path) {
  Settings* new_settings;

  // If settings file exists (and is writable) in executable file working directory
  // (in subdirectory APP_CFG_PATH), then use it (portable settings).
  // Otherwise use settings file stored in home path.
  const SettingsProperties properties = determineProperties(app_path, user_path);

  // Portable settings are available, use them.
  new_settings = new Settings(properties.m_absoluteSettingsFileName, QSettings::IniFormat, properties.m_type, parent);

  // Check if portable settings are available.
  if (properties.m_type == SettingsType::Portable) {
    qDebug("Initializing settings in '%s' (portable way).", qPrintable(QDir::toNativeSeparators(properties.m_absoluteSettingsFileName)));
  }
  else {
    qDebug("Initializing settings in '%s' (non-portable way).",
           qPrintable(QDir::toNativeSeparators(properties.m_absoluteSettingsFileName)));
  }

  return new_settings;
}

SettingsProperties Settings::determineProperties(const QString& app_path, const QString& user_path) {
  SettingsProperties properties;

  properties.m_settingsSuffix = QDir::separator() + APP_CFG_FILE;

  // We will use PORTABLE settings only and only if it is available and NON-PORTABLE
  // settings was not initialized before.
#if defined (Q_OS_LINUX) || defined (Q_OS_ANDROID) || defined (Q_OS_MACOS)

  // DO NOT use portable settings for Linux, it is really not used on that platform.
  const bool will_we_use_portable_settings = false;
#else
  const QString exe_path = qApp->applicationDirPath();
  const QString home_path_file = user_path + properties.m_settingsSuffix;
  const bool portable_settings_available = IOFactory::isFolderWritable(exe_path);
  const bool non_portable_settings_exist = QFile::exists(home_path_file);
  const bool will_we_use_portable_settings = portable_settings_available && !non_portable_settings_exist;
#endif

  if (will_we_use_portable_settings) {
    properties.m_type = SettingsType::Portable;
    properties.m_baseDirectory = app_path;
  }
  else {
    properties.m_type = SettingsType::NonPortable;
    properties.m_baseDirectory = user_path;
  }

  properties.m_absoluteSettingsFileName = properties.m_baseDirectory + properties.m_settingsSuffix;
  return properties;
}
