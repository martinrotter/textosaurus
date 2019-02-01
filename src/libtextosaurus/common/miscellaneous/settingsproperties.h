#ifndef SETTINGSPROPERTIES_H
#define SETTINGSPROPERTIES_H

#include <QString>

enum class SettingsType {
  Portable,
  NonPortable,
  Forced
};

struct SettingsProperties {
  SettingsType m_type;
  QString m_baseDirectory;
  QString m_settingsSuffix;
  QString m_absoluteSettingsFileName;
};

#endif // SETTINGSPROPERTIES_H
