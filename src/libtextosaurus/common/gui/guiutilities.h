// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifndef GUIUTILITIES_H
#define GUIUTILITIES_H

#include <QIcon>
#include <QLabel>
#include <QWidget>

class GuiUtilities {
  public:
    GuiUtilities() = delete;
    ~GuiUtilities() = delete;

    static void disableCloseButton(QWidget& widget);
    static void setLabelAsNotice(QLabel& label, bool is_warning);
    static void applyDialogProperties(QWidget& widget, const QIcon& icon = QIcon(), const QString& title = QString());
    static void applyResponsiveDialogResize(QWidget& widget, double factor = 0.6);
};

#endif // GUIUTILITIES_H
