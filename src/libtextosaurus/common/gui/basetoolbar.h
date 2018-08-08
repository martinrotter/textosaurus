// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifndef TOOLBAR_H
#define TOOLBAR_H

#include <QToolBar>

class TEXTOSAURUS_DLLSPEC BaseBar {
  public:
    BaseBar() = default;
    BaseBar(const BaseBar&) = delete;
    BaseBar& operator=(const BaseBar&) = delete;

    virtual ~BaseBar() = default;

    // Returns all actions which can be added to the toolbar.
    virtual QList<QAction*> availableActions() const = 0;

    // Returns all changeable actions which are currently included
    // in the toolbar.
    virtual QList<QAction*> changeableActions() const = 0;

    // Sets new "actions" to the toolbar and perhaps saves the toolbar
    // state into the settings.
    virtual void saveChangeableActions(const QStringList& actions) = 0;

    // Returns list of default actions.
    virtual QStringList defaultActions() const = 0;
    virtual QStringList savedActions() const = 0;

    // Loads the toolbar state from settings.
    virtual void loadSavedActions();
    virtual QList<QAction*> getSpecificActions(const QStringList& actions) = 0;
    virtual void loadSpecificActions(const QList<QAction*>& actions) = 0;

  protected:
    QAction* findMatchingAction(const QString& action, const QList<QAction*>& actions) const;
};

class TEXTOSAURUS_DLLSPEC BaseToolBar : public QToolBar, public BaseBar {
  Q_OBJECT

  public:
    explicit BaseToolBar(const QString& title, QWidget* parent = nullptr);
    virtual ~BaseToolBar() = default;
};

#endif // TOOLBAR_H
