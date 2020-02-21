// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifndef CLIPBOARDSIDEBAR_H
#define CLIPBOARDSIDEBAR_H

#include "saurus/gui/sidebars/basesidebar.h"

class ClipboardPlugin;
class ClipboardModel;
class ClipboardItem;
class TextEditor;
class QTreeView;

class ClipboardSidebar : public BaseSidebar {
  Q_OBJECT

  friend class ClipboardPlugin;

  public:
    explicit ClipboardSidebar(ClipboardPlugin* plugin, QWidget* parent = nullptr);

    virtual Qt::DockWidgetArea initialArea() const override;
    virtual bool initiallyVisible() const override;
    virtual int initialWidth() const override;

  protected:
    virtual void load() override;

  private slots:
    void onEntryActivated(const QModelIndex& idx);
    void importNewestText();

  private:
    void importSelectedClipboardEntry(ClipboardItem* entry, TextEditor* editor);

  private:
    QTreeView* m_treeClipboard;
    QWidget* m_mainForm;
    ClipboardModel* m_model;
    QAction* m_actionInsertLatestText;
};

#endif // CLIPBOARDSIDEBAR_H
