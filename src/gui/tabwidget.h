// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef TABWIDGET_H
#define TABWIDGET_H

#include <QTabWidget>

#include "gui/tabbar.h"

#include <QUrl>

class QMenu;
class PlainToolButton;
class TextEditor;

class TabWidget : public QTabWidget {
  Q_OBJECT

  public:

    // Constructors and destructors.
    explicit TabWidget(QWidget* parent = nullptr);
    virtual ~TabWidget();

    // Manimulators for tabs.
    int addTab(QWidget* widget, const QString&,
               TabBar::TabType type = TabBar::TabType::TextEditor);
    int addTab(QWidget* widget, const QIcon& icon,
               const QString& label, TabBar::TabType type = TabBar::TabType::TextEditor);
    int insertTab(int index, QWidget* widget, const QString& label,
                  TabBar::TabType type = TabBar::TabType::TextEditor);
    int insertTab(int index, QWidget* widget, const QIcon& icon,
                  const QString& label, TabBar::TabType type = TabBar::TabType::TextEditor);
    void removeTab(int index, bool clear_from_memory);

    TextEditor* textEditorAt(int index) const;

    // Returns tab bar.
    TabBar* tabBar() const;

  public slots:
    bool closeTab(int index);
    void closeAllTabsExceptCurrent();
    void closeAllTabs();

    void gotoNextTab();
    void gotoPreviousTab();

  protected:
    void createConnections();

  private slots:

    // Changes icon/text of the tab.
    void changeTitle(int index, const QString& new_title);
    void changeIcon(int index, const QIcon& new_icon);

  private:
    void indentTabText(int index);
};

inline TabBar* TabWidget::tabBar() const {
  return static_cast<TabBar*>(QTabWidget::tabBar());
}

#endif // TABWIDGET_H
