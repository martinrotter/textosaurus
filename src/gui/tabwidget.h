// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef TABWIDGET_H
#define TABWIDGET_H

#include <QTabWidget>

#include "gui/tabbar.h"

#include <QUrl>

class QMenu;
class PlainToolButton;
class FeedMessageViewer;
class TextEditor;

class TabWidget : public QTabWidget {
  Q_OBJECT

  public:

    // Constructors and destructors.
    explicit TabWidget(QWidget* parent = nullptr);
    virtual ~TabWidget();

    // Manimulators for tabs.
    int addTab(QWidget* widget, const QString&,
               const TabBar::TabType& type = TabBar::NonClosable);
    int addTab(QWidget* widget, const QIcon& icon,
               const QString& label, const TabBar::TabType& type = TabBar::NonClosable);
    int insertTab(int index, QWidget* widget, const QString& label,
                  const TabBar::TabType& type = TabBar::Closable);
    int insertTab(int index, QWidget* widget, const QIcon& icon,
                  const QString& label, const TabBar::TabType& type = TabBar::NonClosable);
    void removeTab(int index, bool clear_from_memory);

    TextEditor* textEditorAt(int index) const;

    // Returns tab bar.
    TabBar* tabBar() const;

    // Sets up icons for this TabWidget.
    void setupIcons();

  public slots:

    // Called when number of tab pages changes.
    void checkTabBarVisibility();

    // Tab closing.
    bool closeTab(int index);
    void closeAllTabsExceptCurrent();
    void closeAllTabs();

    // Displays download manager.
    void showDownloadManager();

    void gotoNextTab();
    void gotoPreviousTab();

  protected:

    // Creates necesary connections.
    void createConnections();

    // Sets up properties of custom corner button.
    void setupMainMenuButton();

    // Handlers of insertin/removing of tabs.
    void tabInserted(int index);
    void tabRemoved(int index);

  private slots:

    // Changes icon/text of the tab.
    void changeTitle(int index, const QString& new_title);
    void changeIcon(int index, const QIcon& new_icon);

    // Opens main menu.
    void openMainMenu();

  private:
    void indentTabText(int index);

    PlainToolButton* m_btnMainMenu;
    QMenu* m_menuMain;
};

inline TabBar* TabWidget::tabBar() const {
  return static_cast<TabBar*>(QTabWidget::tabBar());
}

#endif // TABWIDGET_H
