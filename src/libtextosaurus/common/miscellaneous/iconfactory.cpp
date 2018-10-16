// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "common/miscellaneous/iconfactory.h"

#include <QBuffer>
#include <QDebug>

IconFactory::IconFactory(QObject* parent) : QObject(parent) {}

QIcon IconFactory::fromTheme(const QString& name) {
  return QIcon::fromTheme(name);
}

QIcon IconFactory::miscIcon(const QString& name) {
  return QIcon(QString(APP_THEME_PATH) + QDir::separator() + QL1S("misc") + QDir::separator() + name + QL1S(".png"));
}

void IconFactory::setupSearchPaths() {
  QIcon::setThemeSearchPaths(QIcon::themeSearchPaths() << APP_THEME_PATH);
  qDebug().noquote().nospace() << QSL("Available icon theme paths: ")
                               << QIcon::themeSearchPaths()
                               << QSL(".");
}

void IconFactory::loadIconTheme(const QString& theme_name) {
  const QStringList installed_themes = installedIconThemes();

  if (currentIconTheme() == theme_name) {
    qDebug().noquote().nospace() << QSL("Icon theme '") << theme_name << QSL("' already loaded.");
    return;
  }

  // Display list of installed themes.
  qDebug().noquote().nospace() << "Installed icon themes: "
                               << QStringList(installed_themes)
                               << QSL(".");

  if (installed_themes.contains(theme_name)) {
    // Desired icon theme is installed and can be loaded.
    qDebug().noquote().nospace() << QSL("Loading icon theme '") << theme_name << QSL("'.");
    QIcon::setThemeName(theme_name);
  }
  else {
    // Desired icon theme is not currently available.
    // Install "default" icon theme instead.
    qWarning().noquote().nospace()
      << QSL("Icon theme '")
      << theme_name
      << QSL("' cannot be loaded because it is not installed. No icon theme (or default icon theme) is loaded now.");
    QIcon::setThemeName(APP_NO_THEME);
  }
}

QStringList IconFactory::installedIconThemes() const {
  QStringList icon_theme_names;

  icon_theme_names << APP_NO_THEME;

  // Iterate all directories with icon themes.
  QStringList icon_themes_paths = QIcon::themeSearchPaths();
  QStringList filters_index;

  filters_index.append("index.theme");
  icon_themes_paths.removeDuplicates();

  foreach (const QString& icon_path, icon_themes_paths) {
    const QDir icon_dir(icon_path);

    // Iterate all icon themes in this directory.
    foreach (const QFileInfo& icon_theme_path, icon_dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot |
                                                                      QDir::Readable | QDir::CaseSensitive |
                                                                      QDir::NoSymLinks,
                                                                      QDir::Time)) {
      QDir icon_theme_dir = QDir(icon_theme_path.absoluteFilePath());

      if (icon_theme_dir.exists(filters_index.at(0))) {
        icon_theme_names << icon_theme_dir.dirName();
      }
    }
  }

  icon_theme_names.removeDuplicates();
  return icon_theme_names;
}
