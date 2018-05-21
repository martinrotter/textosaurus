// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "common/dynamic-shortcuts/dynamicshortcuts.h"

#include "common/miscellaneous/settings.h"
#include "definitions/definitions.h"
#include "saurus/miscellaneous/application.h"

#include <QAction>

// TODO: oddělat referenci na saurus/application, přidat settings jako parametr.
void DynamicShortcuts::save(const QList<QAction*>& actions) {
  Settings* settings = qApp->settings();

  for (const QAction* action : actions) {
    settings->setValue(GROUP(Keyboard), action->objectName(), action->shortcut().toString(QKeySequence::PortableText));
  }
}

void DynamicShortcuts::load(const QList<QAction*>& actions) {
  Settings* settings = qApp->settings();

  for (QAction* action : actions) {
    QString shortcut_for_action = settings->value(GROUP(Keyboard),
                                                  action->objectName(),
                                                  action->shortcut().toString(QKeySequence::PortableText)).toString();

    action->setShortcutContext(Qt::ApplicationShortcut);
    action->setShortcut(QKeySequence::fromString(shortcut_for_action, QKeySequence::PortableText));
  }
}
