#ifndef FILESYSTEMVIEW_H
#define FILESYSTEMVIEW_H

#include <QListView>

class FilesystemView : public QListView {
  Q_OBJECT

  public:
    explicit FilesystemView(QWidget* parent = nullptr);
    virtual ~FilesystemView() = default;

    virtual void setRootIndex(const QModelIndex& index) override;

  signals:
    void rootIndexChanged(const QModelIndex& index);
};

#endif // FILESYSTEMVIEW_H
