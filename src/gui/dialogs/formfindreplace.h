#ifndef FORMFINDREPLACE_H
#define FORMFINDREPLACE_H

#include <QDialog>

namespace Ui {
class FormFindReplace;
}

class FormFindReplace : public QDialog
{
    Q_OBJECT

public:
    explicit FormFindReplace(QWidget *parent = 0);
    ~FormFindReplace();

private:
    Ui::FormFindReplace *ui;
};

#endif // FORMFINDREPLACE_H
