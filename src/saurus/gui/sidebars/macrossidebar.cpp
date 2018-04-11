#include "macrossidebar.h"
#include "ui_macrossidebar.h"

MacrosSidebar::MacrosSidebar(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::MacrosSidebar)
{
  ui->setupUi(this);
}

MacrosSidebar::~MacrosSidebar()
{
  delete ui;
}
