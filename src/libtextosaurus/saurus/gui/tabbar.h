// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifndef TABBAR_H
#define TABBAR_H

#include <QTabBar>

class TabBar : public QTabBar {
  Q_OBJECT

  public:
    explicit TabBar(QWidget* parent = nullptr);
    virtual ~TabBar() = default;

    // Getter/setter for tab type.
    void setupTabControls(int index);

  private slots:
    void closeTabViaButton();

  private:
    virtual void mouseDoubleClickEvent(QMouseEvent* event) override;
    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void wheelEvent(QWheelEvent* event) override;

  signals:
    void emptySpaceDoubleClicked();
};

#endif // TABBAR_H
