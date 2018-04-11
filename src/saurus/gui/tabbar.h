// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef TABBAR_H
#define TABBAR_H

#include "common/miscellaneous/iconfactory.h"

#include <QTabBar>
#include <QVariant>

enum class TabType {
  TextEditor = 1
};

class TabBar : public QTabBar {
  Q_OBJECT

  public:
    explicit TabBar(QWidget* parent = nullptr);
    virtual ~TabBar() = default;

    // Getter/setter for tab type.
    void setTabType(int index, TabType type = TabType::TextEditor);
    TabType tabType(int index) const;

  private slots:
    void closeTabViaButton();

  private:
    virtual void mouseDoubleClickEvent(QMouseEvent* event) override;
    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void wheelEvent(QWheelEvent* event) override;

  signals:
    void emptySpaceDoubleClicked();
};

inline TabType TabBar::tabType(int index) const {
  return static_cast<TabType>(tabData(index).value<int>());
}

Q_DECLARE_METATYPE(TabType)

#endif // TABBAR_H
