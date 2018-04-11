#ifndef MACROSSIDEBAR_H
#define MACROSSIDEBAR_H

#include <QWidget>

namespace Ui {
  class MacrosSidebar;
}

class MacrosSidebar : public QWidget
{
    Q_OBJECT

  public:
    explicit MacrosSidebar(QWidget *parent = 0);
    ~MacrosSidebar();

  private:
    Ui::MacrosSidebar *ui;
};

#endif // MACROSSIDEBAR_H
