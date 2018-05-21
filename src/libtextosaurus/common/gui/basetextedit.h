// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifndef BASETEXTEDIT_H
#define BASETEXTEDIT_H

#include <QPlainTextEdit>

class BaseTextEdit : public QPlainTextEdit {
  Q_OBJECT

  public:
    explicit BaseTextEdit(QWidget* parent = nullptr);
    virtual ~BaseTextEdit() = default;

  public slots:
    void submit(const QString& text);

  protected:
    virtual void keyPressEvent(QKeyEvent* event) override;

  signals:

    // Emitted if user hits ENTER button.
    void submitted(const QString& text);
};

#endif // BASETEXTEDIT_H
