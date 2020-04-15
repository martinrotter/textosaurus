// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifndef MESSAGEBOX_H
#define MESSAGEBOX_H

#include <QMessageBox>

#include <QDialogButtonBox>
#include <QFileDialog>

class MessageBox : public QMessageBox {
  Q_OBJECT

  public:
    explicit MessageBox(QWidget* parent = nullptr);
    virtual ~MessageBox() = default;

    // Custom icon setting.
    void setIcon(Icon icon);

    static void setCheckBox(QMessageBox* msg_box, const QString& text, bool* data);
    static QString getExistingDirectory(QWidget* parent, const QString& caption, const QString& initial_dir);
    static QString getOpenFileName(QWidget* parent, const QString& caption, const QString& initial_dir,
                                   const QStringList& filters, QString* selected_filter = nullptr);
    static QString getSaveFileName(QWidget* parent, const QString& caption, const QString& initial_dir,
                                   const QStringList& filters, QString* selected_filter = nullptr);

    // Displays custom message box.
    static QMessageBox::StandardButton show(QWidget* parent,
                                            QMessageBox::Icon icon,
                                            const QString& title,
                                            const QString& text,
                                            const QString& informative_text = QString(),
                                            const QString& detailed_text = QString(),
                                            QMessageBox::StandardButtons buttons = QMessageBox::Ok,
                                            QMessageBox::StandardButton default_button = QMessageBox::Ok,
                                            bool* dont_show_again = nullptr,
                                            const QString& dont_show_again_text = QString());
    static QIcon iconForStatus(QMessageBox::Icon status);

  private:
    static QFileDialog* getFileDialog(QWidget* parent, const QString& caption, const QString& initial_dir,
                                      const QStringList& filters, QFileDialog::FileMode mode);
};

#endif // MESSAGEBOX_H
