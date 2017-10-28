// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef MAIN_TOOLBAR_H
#define MAIN_TOOLBAR_H

#include "gui/basetoolbar.h"

class ToolBar : public BaseToolBar {
  Q_OBJECT

  public:

    // Constructors and destructors.
    explicit ToolBar(const QString& title, QWidget* parent = nullptr);
    virtual ~ToolBar();

    QList<QAction*> availableActions() const;
    QList<QAction*> changeableActions() const;
    void saveChangeableActions(const QStringList& actions);

    QList<QAction*> getSpecificActions(const QStringList& actions);
    void loadSpecificActions(const QList<QAction*>& actions);

    QStringList defaultActions() const;
    QStringList savedActions() const;

    void refreshVisualProperties();
};

#endif // MAIN_TOOLBAR_H
