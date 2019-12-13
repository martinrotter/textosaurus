// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "common/gui/guiutilities.h"

#include "definitions/definitions.h"

#if defined (Q_OS_ANDROID)
#include <QApplication>
#include <QDesktopWidget>
#include <QSize>
#endif

void GuiUtilities::disableCloseButton(QWidget& widget) {
  Qt::WindowFlags flags = widget.windowFlags();
  Qt::WindowFlags closeFlag = Qt::WindowType::WindowCloseButtonHint;
  flags = flags & (~closeFlag);
  widget.setWindowFlags(flags);
}

void GuiUtilities::setLabelAsNotice(QLabel& label, bool is_warning) {
  label.setMargin(4);

  if (is_warning) {
    label.setStyleSheet(QSL("font-weight: bold; font-style: italic; color: red"));
  }
  else {
    label.setStyleSheet(QSL("font-style: italic;"));
  }
}

void GuiUtilities::applyDialogProperties(QWidget& widget, const QIcon& icon, const QString& title) {
  widget.setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
  widget.setWindowIcon(icon);

  if (!title.isEmpty()) {
    widget.setWindowTitle(title);
  }
}

void GuiUtilities::applyResponsiveDialogResize(QWidget& widget, double factor) {
#if defined (Q_OS_ANDROID)
  auto desktop_geom = QApplication::desktop()->screenGeometry();
  auto ratio = double(widget.size().height()) / widget.size().width();
  int widt = desktop_geom.width() * factor;
  int heig = widt * ratio;

  widget.resize(widt, heig);
#else
  Q_UNUSED(factor)
  Q_UNUSED(widget)
#endif
}
