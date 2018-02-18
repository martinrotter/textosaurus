// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef FILESYSTEMVIEW_H
#define FILESYSTEMVIEW_H

#include <QListView>

class QKeyEvent;
class FilesystemModel;

class FilesystemView : public QListView {
  Q_OBJECT

  public:
    explicit FilesystemView(FilesystemModel* model, QWidget* parent = nullptr);
    virtual ~FilesystemView() = default;

    FilesystemModel* model() const;
    virtual void setRootIndex(const QModelIndex& index) override;

  public slots:
    void cdUp();

  protected:
    virtual void keyPressEvent(QKeyEvent* event) override;

  signals:
    void rootIndexChanged(const QModelIndex& index);

  private:
    FilesystemModel* m_model;
};

#endif // FILESYSTEMVIEW_H
