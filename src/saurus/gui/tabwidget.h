// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef TABWIDGET_H
#define TABWIDGET_H

#include <QTabWidget>

#include "saurus/gui/tabbar.h"

#include <QUrl>

class QMenu;
class PlainToolButton;
class TextEditor;
class Tab;

class TabWidget : public QTabWidget {
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

    int indexOfEditor(TextEditor* editor) const;
    TextEditor* textEditorAt(int index) const;
    Tab* tabAt(int index) const;

    QList<TextEditor*> editors()  const;
    QList<Tab*> tabs() const;

    // Returns tab bar.
    TabBar* tabBar() const;

  public slots:
    bool closeTab(int index);
    void closeAllTabsExceptCurrent();
    void closeAllTabs();
    void closeCurrentTab();

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
