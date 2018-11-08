// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifndef TAB_H
#define TAB_H

#include <QWidget>

#include <QIcon>
#include <QList>
#include <QMenu>

class TextEditor;
class TabWidget;

class Tab : public QWidget {
  Q_OBJECT

  public:
    enum class TabType {
      TextEditor = 1,
      Other = 2
    };

    Q_ENUM(TabType)

    explicit Tab(TabWidget* tab_widget);

    // Returns context menu.
    // NOTE: Caller must take ownership of the menu.
    virtual QMenu* contextMenu() const = 0;

    // Returns primary editor.
    virtual TextEditor* primaryEditor() const = 0;

    // Returns list of all editors, sorted from
    // most significant to less significant.
    virtual QList<TextEditor*> allEditors() const = 0;
    virtual TabType tabType() const = 0;

    // Return count of editors.
    virtual int countOfEditors() const = 0;

    // Title and icon of a tab.
    virtual QString title() const = 0;
    virtual QIcon icon() const = 0;
    virtual QString toolTip() const = 0;

    // Returns index of this tab in tab widget.
    // NOTE: Returns value < 0 if this tab is not in tab widget.
    int index() const;

  public slots:
    void requestVisibility();

  signals:
    void iconChanged(QIcon icon);
    void titleChanged(QString title, QString tool_tip);
    void visibilityRequested();

  private:
    TabWidget* m_tabWidget;
};

Q_DECLARE_METATYPE(Tab::TabType)

#endif // TAB_H
