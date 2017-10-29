// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef STATUSBAR_H
#define STATUSBAR_H

#include <QStatusBar>

class StatusBar : public QStatusBar {
  Q_OBJECT

  public:
    explicit StatusBar(QWidget* parent = nullptr);
    virtual ~StatusBar();

  protected:
    bool eventFilter(QObject* watched, QEvent* event);
};

#endif // STATUSBAR_H
