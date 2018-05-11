// For license of this file, see <project-root-folder>/LICENSE.md.

#include "common/gui/basetextedit.h"

#include <QKeyEvent>

BaseTextEdit::BaseTextEdit(QWidget* parent) : QPlainTextEdit(parent) {}

void BaseTextEdit::submit(const QString& text) {
  setPlainText(text);
  emit submitted(text);
}

void BaseTextEdit::keyPressEvent(QKeyEvent* event) {
  if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) {
    emit submitted(toPlainText());

    event->accept();
  }
  else {
    QPlainTextEdit::keyPressEvent(event);
  }
}
