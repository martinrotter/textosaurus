// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifndef DYNAMICSHORTCUTS_H
#define DYNAMICSHORTCUTS_H

#include <QList>

#include "definitions/definitions.h"

class QAction;
class Settings;

class TEXTOSAURUS_DLLSPEC DynamicShortcuts {
  public:
    DynamicShortcuts() = delete;
    ~DynamicShortcuts() = delete;

    // Checks the application settings and then initializes shortcut of
    // each action from actions from the settings.
    static void load(const QList<QAction*>& actions, Settings* settings);

    // Stores shortcut of each action from actions into the application
    // settings.
    static void save(const QList<QAction*>& actions, Settings* settings);
};

#endif // DYNAMICSHORTCUTS_H
