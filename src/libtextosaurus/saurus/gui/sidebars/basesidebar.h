// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include <QDockWidget>

#ifndef BASESIDEBAR_H
#define BASESIDEBAR_H

class TextApplication;

class BaseSidebar : public QDockWidget {
  Q_OBJECT

  public:
    explicit BaseSidebar(TextApplication* text_app, QWidget* parent = nullptr);
    virtual ~BaseSidebar() = default;

    QAction* generateAction();
    virtual Qt::DockWidgetArea initialArea() const = 0;
    virtual bool initiallyVisible() const = 0;
    virtual int initialWidth() const = 0;

  public slots:
    void switchVisibility();
    virtual void load() = 0;

  protected:
    virtual void showEvent(QShowEvent* event) override;

  protected:
    TextApplication* m_textApp;
};

#endif // BASESIDEBAR_H
