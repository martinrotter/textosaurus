// For license of this file, see <project-root-folder>/LICENSE.md.

#include "gui/toolbar.h"

#include "miscellaneous/application.h"
#include "miscellaneous/iconfactory.h"
#include "miscellaneous/settings.h"

#include <QWidgetAction>

ToolBar::ToolBar(const QString& title, QWidget* parent) : BaseToolBar(title, parent) {
  setFloatable(false);
  setMovable(false);
  refreshVisualProperties();
  loadSavedActions();
}

ToolBar::~ToolBar() {}

QList<QAction*> ToolBar::availableActions() const {
  return qApp->userActions();
}

QList<QAction*> ToolBar::changeableActions() const {
  return actions();
}

void ToolBar::saveChangeableActions(const QStringList& actions) {
  qApp->settings()->setValue(GROUP(GUI), GUI::ToolbarActions, actions.join(QSL(",")));
  loadSpecificActions(getSpecificActions(actions));
}

QList<QAction*> ToolBar::getSpecificActions(const QStringList& actions) {
  QList<QAction*> available_actions = availableActions();
  QList<QAction*> spec_actions;

  // Iterate action names and add respectable actions into the toolbar.
  foreach (const QString& action_name, actions) {
    QAction* matching_action = findMatchingAction(action_name, available_actions);

    if (matching_action != nullptr) {
      // Add existing standard action.
      spec_actions.append(matching_action);
    }
    else if (action_name == SEPARATOR_ACTION_NAME) {
      // Add new separator.
      QAction* act = new QAction(this);

      act->setSeparator(true);
      spec_actions.append(act);
    }
    else if (action_name == SPACER_ACTION_NAME) {
      // Add new spacer.
      QWidget* spacer = new QWidget(this);

      spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
      QWidgetAction* action = new QWidgetAction(this);

      action->setDefaultWidget(spacer);
      action->setIcon(qApp->icons()->fromTheme(QSL("system-search")));
      action->setProperty("type", SPACER_ACTION_NAME);
      action->setProperty("name", tr("Toolbar spacer"));
      spec_actions.append(action);
    }
  }

  return spec_actions;
}

void ToolBar::loadSpecificActions(const QList<QAction*>& actions) {
  clear();

  foreach (QAction* act, actions) {
    addAction(act);
  }
}

QStringList ToolBar::defaultActions() const {
  return QString(GUI::ToolbarActionsDef).split(QL1C(','),
                                               QString::SkipEmptyParts);
}

QStringList ToolBar::savedActions() const {
  return qApp->settings()->value(GROUP(GUI), SETTING(GUI::ToolbarActions)).toString().split(',',
                                                                                            QString::SkipEmptyParts);
}

void ToolBar::refreshVisualProperties() {
  const Qt::ToolButtonStyle button_style = static_cast<Qt::ToolButtonStyle>(qApp->settings()->value(GROUP(GUI),
                                                                                                    SETTING(GUI::ToolbarStyle)).toInt());

  setToolButtonStyle(button_style);
}
