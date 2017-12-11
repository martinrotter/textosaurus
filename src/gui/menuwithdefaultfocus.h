// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef MENUWITHDEFAULTFOCUS_H
#define MENUWITHDEFAULTFOCUS_H

#include <QMenu>

class MenuWithDefaultFocus : public QMenu {
  Q_OBJECT

  public:
    explicit MenuWithDefaultFocus(QWidget* parent) : QMenu(parent) {}

  protected:
    bool focusNextPrevChild(bool next);

};

inline bool MenuWithDefaultFocus::focusNextPrevChild(bool next) {
  return QWidget::focusNextPrevChild(next);
}

#endif // MENUWITHDEFAULTFOCUS_H
