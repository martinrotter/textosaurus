// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifndef ICONFACTORY_H
#define ICONFACTORY_H

#include <QObject>

#include "definitions/definitions.h"

#include <QDir>
#include <QHash>
#include <QIcon>
#include <QString>

class TEXTOSAURUS_DLLSPEC IconFactory : public QObject {
  Q_OBJECT

  public:
    explicit IconFactory(QObject* parent = nullptr);
    virtual ~IconFactory() = default;

    // Returns icon from active theme or invalid icon if
    // "no icon theme" is set.
    QIcon fromTheme(const QString& name);
    QIcon miscIcon(const QString& name);

    // Adds custom application path to be search for icons.
    void setupSearchPaths();

    // Returns list of installed themes, including "default" theme.
    QStringList installedIconThemes() const;

    // Loads name of selected icon theme (from settings) for the application and
    // activates it. If that particular theme is not installed, then
    // "default" theme is loaded.
    void loadIconTheme(const QString& theme_name);

    // Returns name of currently activated theme for the application.
    QString currentIconTheme() const;
};

inline QString IconFactory::currentIconTheme() const {
  return QIcon::themeName();
}

#endif // ICONFACTORY_H
