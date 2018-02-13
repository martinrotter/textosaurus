// For license of this file, see <project-root-folder>/LICENSE.md.

#include "miscellaneous/iconfactory.h"

#include <QBuffer>

IconFactory::IconFactory(QObject* parent) : QObject(parent) {}

QIcon IconFactory::fromTheme(const QString& name) {
  return QIcon::fromTheme(name);
}

QPixmap IconFactory::miscPixmap(const QString& name) {
  return QPixmap(QString(APP_THEME_PATH) + QDir::separator() + "misc" + QDir::separator() + name + ".png");
}

QIcon IconFactory::miscIcon(const QString& name) {
  return QIcon(QString(APP_THEME_PATH) + QDir::separator() + "misc" + QDir::separator() + name + ".png");
}

void IconFactory::setupSearchPaths() {
  QIcon::setThemeSearchPaths(QIcon::themeSearchPaths() << APP_THEME_PATH);
  qDebug("Available icon theme paths: %s.",
         qPrintable(QIcon::themeSearchPaths()
                    .replaceInStrings(QRegExp(QSL("^|$")), QSL("\'"))
                    .replaceInStrings(QRegExp(QSL("/")), QDir::separator()).join(QSL(", "))));
}

void IconFactory::loadIconTheme(const QString& theme_name) {
  const QStringList installed_themes = installedIconThemes();

  if (currentIconTheme() == theme_name) {
    qDebug("Icon theme '%s' already loaded.", qPrintable(theme_name));
    return;
  }

  // Display list of installed themes.
  qDebug("Installed icon themes are: %s.",
         qPrintable(QStringList(installed_themes)
                    .replaceInStrings(QRegExp(QSL("^|$")), QSL("\'"))
                    .replaceInStrings(QRegExp(QSL("^\\'$")), QSL("\'\'")).join(QSL(", "))));

  if (installed_themes.contains(theme_name)) {
    // Desired icon theme is installed and can be loaded.
    qDebug("Loading icon theme '%s'.", qPrintable(theme_name));
    QIcon::setThemeName(theme_name);
  }
  else {
    // Desired icon theme is not currently available.
    // Install "default" icon theme instead.
    qWarning("Icon theme '%s' cannot be loaded because it is not installed. No icon theme (or default icon theme) is loaded now.",
             qPrintable(theme_name));
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
