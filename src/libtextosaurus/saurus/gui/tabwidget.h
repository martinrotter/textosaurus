// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifndef TABWIDGET_H
#define TABWIDGET_H

#include <QTabWidget>

#include "definitions/definitions.h"
#include "saurus/gui/tabbar.h"

#include <QUrl>

class QMenu;
class PlainToolButton;
class TextEditor;
class Tab;

class TEXTOSAURUS_DLLSPEC TabWidget : public QTabWidget {
  Q_OBJECT

  public:
    explicit TabWidget(QWidget* parent = nullptr);
    virtual ~TabWidget() = default;

    // Manimulators for tabs.
    int addTab(Tab* widget, const QString& label);
    int addTab(Tab* widget, const QIcon& icon, const QString& label);
    int insertTab(int index, Tab* widget, const QString& label);
    int insertTab(int index, Tab* widget, const QIcon& icon, const QString& label);
    bool removeTab(int index, bool clear_from_memory);

    // Changes icon/text of the tab.
    void changeTitle(int index, const QString& new_title, const QString& tool_tip = QString());
    void changeIcon(int index, const QIcon& new_icon);

    // Returns true if there is only one tab visible and it has no contents and is in unmodified state
    // and not bound to file.
    bool hasOnlyOneEmptyEditor() const;

    TextEditor* currentEditor() const;
    Tab* currentTab() const;

    int indexOfEditor(TextEditor* editor) const;
    TextEditor* textEditorAt(int index) const;
    Tab* tabAt(int index) const;

    QList<TextEditor*> editors()  const;
    QList<Tab*> tabs() const;

    Tab* tabWithFile(const QString& file_path) const;

    // Returns tab bar.
    TabBar* tabBar() const;

  public slots:
    bool closeTab(int index);
    void closeAllTabsExceptCurrent();
    void closeAllTabs();
    void closeCurrentTab();

    void gotoNextTab();
    void gotoPreviousTab();

    void makeTabVisible(Tab* tab);
    void makeEditorVisible(TextEditor* editor);

  protected:
    void createConnections();

  private slots:
    void onTabRequestedVisibility();
    void showTabContextMenu(const QPoint& point);

  private:
    void indentTabText(int index);
    void prepareNewTab(int index);
};

inline TabBar* TabWidget::tabBar() const {
  return static_cast<TabBar*>(QTabWidget::tabBar());
}

#endif // TABWIDGET_H
