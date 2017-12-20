// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef MESSAGEBOX_H
#define MESSAGEBOX_H

#include <QDialogButtonBox>
#include <QMessageBox>

class MessageBox : public QMessageBox {
  Q_OBJECT

  public:
    explicit MessageBox(QWidget* parent = nullptr);
    virtual ~MessageBox() = default;

    // Custom icon setting.
    void setIcon(Icon icon);

    static void setCheckBox(QMessageBox* msg_box, const QString& text, bool* data);
    static QString getSaveFileName(QWidget* parent, const QString& caption, const QString& initial_dir,
                                   const QString& preselected_file, const QStringList& filters, QString* selected_filter);

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
};

#endif // MESSAGEBOX_H
