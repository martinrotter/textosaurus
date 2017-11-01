// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef CLOSEBUTTON_H
#define CLOSEBUTTON_H

#include <QToolButton>

class PlainToolButton : public QToolButton {
  Q_OBJECT

  public:

    // Contructors and destructors.
    explicit PlainToolButton(QWidget* parent = nullptr);
    virtual ~PlainToolButton();

  public slots:
    void setPadding(int left, int top, int right, int bottom);
    void setChecked(bool checked);
    void reactOnActionChange(QAction* action);
    void reactOnSenderActionChange();

  protected:

    // Custom look.
    void paintEvent(QPaintEvent* e);

  private:
    int m_paddingLeft = 0;
    int m_paddingTop = 0;
    int m_paddingRight = 0;
    int m_paddingBottom = 0;
};

#endif // CLOSEBUTTON_H
