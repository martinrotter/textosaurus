// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifndef CLIPBOARDSIDEBAR_H
#define CLIPBOARDSIDEBAR_H

#include "saurus/gui/sidebars/basesidebar.h"

class ClipboardPlugin;
class ClipboardModel;
class QTreeView;

class ClipboardSidebar : public BaseSidebar {
  Q_OBJECT

  public:
    explicit ClipboardSidebar(ClipboardPlugin* plugin, QWidget* parent = nullptr);

    virtual Qt::DockWidgetArea initialArea() const override;
    virtual bool initiallyVisible() const override;
    virtual int initialWidth() const override;

  protected:
    virtual void load() override;

  private:
    QTreeView* m_treeClipboard;
    ClipboardModel* m_model;
};

#endif // CLIPBOARDSIDEBAR_H
