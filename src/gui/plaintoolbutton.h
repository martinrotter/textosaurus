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

    // Padding changers.
    void setPadding(int left, int top, int right, int bottom);

  public slots:
    void setChecked(bool checked);
    void reactOnActionChange(QAction* action);
    void reactOnSenderActionChange();

  protected:

    // Custom look.
    void paintEvent(QPaintEvent* e);

  private:
    int m_paddingLeft;
    int m_paddingTop;
    int m_paddingRight;
    int m_paddingBottom;
};

#endif // CLOSEBUTTON_H
