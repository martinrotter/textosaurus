// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef STATUSBAR_H
#define STATUSBAR_H

#include <QStatusBar>

#include "gui/basetoolbar.h"

class QProgressBar;
class PlainToolButton;
class QLabel;
class Mutex;

class StatusBar : public QStatusBar {
  Q_OBJECT

  public:
    explicit StatusBar(QWidget* parent = nullptr);
    virtual ~StatusBar();

  public slots:
    void showProgressDownload(int progress, const QString& tooltip);
    void clearProgressDownload();

  protected:
    bool eventFilter(QObject* watched, QEvent* event);

  private:
    QProgressBar* m_barProgressDownload;
    QLabel* m_lblProgressDownload;
};

#endif // STATUSBAR_H
