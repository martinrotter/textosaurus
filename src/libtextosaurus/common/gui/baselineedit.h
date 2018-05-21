// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifndef BASELINEEDIT_H
#define BASELINEEDIT_H

#include <QLineEdit>

class BaseLineEdit : public QLineEdit {
  Q_OBJECT

  public:
    explicit BaseLineEdit(QWidget* parent = nullptr);
    virtual ~BaseLineEdit() = default;

  public slots:
    void submit(const QString& text);

  protected:
    virtual void keyPressEvent(QKeyEvent* event) override;

  signals:

    // Emitted if user hits ENTER button.
    void submitted(const QString& text);
};

#endif // BASELINEEDIT_H
