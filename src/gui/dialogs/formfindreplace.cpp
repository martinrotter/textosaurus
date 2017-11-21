#include "formfindreplace.h"
#include "ui_formfindreplace.h"

FormFindReplace::FormFindReplace(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FormFindReplace)
{
    ui->setupUi(this);
}

FormFindReplace::~FormFindReplace()
{
    delete ui;
}
