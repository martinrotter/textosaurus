// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifndef MAIN_TOOLBAR_H
#define MAIN_TOOLBAR_H

#include "common/gui/basetoolbar.h"

class TEXTOSAURUS_DLLSPEC ToolBar : public BaseToolBar {
  Q_OBJECT

  public:
    explicit ToolBar(const QString& title, QWidget* parent = nullptr);
    virtual ~ToolBar() override = default;

    virtual QList<QAction*> availableActions() const override;
    virtual QList<QAction*> changeableActions() const override;
    virtual void saveChangeableActions(const QStringList& actions) override;
    virtual QList<QAction*> getSpecificActions(const QStringList& actions) override;
    virtual void loadSpecificActions(const QList<QAction*>& actions) override;
    virtual QStringList defaultActions() const override;
    virtual QStringList savedActions() const override;

    void refreshVisualProperties();

    bool isActive() const;

  public slots:
    void setIsActive(bool is_active);
    void resetActiveState();

  private:
    bool m_isActive;
};

#endif // MAIN_TOOLBAR_H
