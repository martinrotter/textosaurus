// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "common/gui/messagebox.h"

#include "common/miscellaneous/iconfactory.h"
#include "saurus/miscellaneous/application.h"

#include <QCheckBox>
#include <QDialogButtonBox>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QPushButton>
#include <QStyle>
#include <QtGlobal>

MessageBox::MessageBox(QWidget* parent) : QMessageBox(parent) {}

void MessageBox::setIcon(QMessageBox::Icon icon) {
  // Determine correct status icon size.
  const int icon_size = qApp->style()->pixelMetric(QStyle::PM_MessageBoxIconSize, nullptr, this);

  // Setup status icon.
  setIconPixmap(iconForStatus(icon).pixmap(icon_size, icon_size));
}

void MessageBox::setCheckBox(QMessageBox* msg_box, const QString& text, bool* data) {
  // Add "don't show this again checkbox.
  QCheckBox* check_box = new QCheckBox(msg_box);

  check_box->setChecked(*data);
  check_box->setText(text);
  connect(check_box, &QCheckBox::toggled, [=](bool checked) {
    *data = checked;
  });
  msg_box->setCheckBox(check_box);
}

QString MessageBox::getSaveFileName(QWidget* parent, const QString& caption, const QString& initial_dir,
                                    const QStringList& filters, QString* selected_filter) {
  QFileDialog dialog(parent, caption, initial_dir, filters.join(QSL(";;")));

  //dialog.setDefaultSuffix(QSL("txt"));
  dialog.setFileMode(QFileDialog::FileMode::AnyFile);
  dialog.setAcceptMode(QFileDialog::AcceptMode::AcceptSave);
  dialog.setOption(QFileDialog::Option::ShowDirsOnly, false);
  dialog.setOption(QFileDialog::Option::DontConfirmOverwrite, false);

  if (dialog.exec() == QDialog::DialogCode::Accepted) {
    if (selected_filter != nullptr) {
      *selected_filter = dialog.selectedNameFilter();
    }

    return dialog.selectedFiles().isEmpty() ? QString() : dialog.selectedFiles().at(0);
  }
  else {
    return QString();
  }
}

QIcon MessageBox::iconForStatus(QMessageBox::Icon status) {
  switch (status) {
    case QMessageBox::Information:
      return qApp->icons()->fromTheme(QSL("dialog-information"));

    case QMessageBox::Warning:
      return qApp->icons()->fromTheme(QSL("dialog-warning"));

    case QMessageBox::Critical:
      return qApp->icons()->fromTheme(QSL("dialog-error"));

    case QMessageBox::Question:
      return qApp->icons()->fromTheme(QSL("dialog-question"));

    default:
      return QIcon();
  }
}

QMessageBox::StandardButton MessageBox::show(QWidget* parent,
                                             QMessageBox::Icon icon,
                                             const QString& title,
                                             const QString& text,
                                             const QString& informative_text,
                                             const QString& detailed_text,
                                             QMessageBox::StandardButtons buttons,
                                             QMessageBox::StandardButton default_button,
                                             bool* dont_show_again,
                                             const QString& dont_show_again_text) {
  // Create and find needed components.
  MessageBox msg_box(parent);

  // Initialize message box properties.
  msg_box.setWindowTitle(title);
  msg_box.setText(text);
  msg_box.setInformativeText(informative_text);
  msg_box.setDetailedText(detailed_text);
  msg_box.setIcon(icon);
  msg_box.setStandardButtons(buttons);
  msg_box.setDefaultButton(default_button);

  if (dont_show_again != nullptr) {
    MessageBox::setCheckBox(&msg_box,
                            dont_show_again_text.isEmpty() ?
                            tr("Do not show this dialog again.") :
                            dont_show_again_text, dont_show_again);
  }

  // Display it.
  if (msg_box.exec() == -1) {
    return QMessageBox::Cancel;
  }
  else {
    return msg_box.standardButton(msg_box.clickedButton());
  }
}
