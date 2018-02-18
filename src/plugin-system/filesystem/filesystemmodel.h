// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef FILESYSTEMSIDEBARMODEL_H
#define FILESYSTEMSIDEBARMODEL_H

#include <QFileSystemModel>

class FilesystemModel : public QFileSystemModel {
  Q_OBJECT

  public:
    explicit FilesystemModel(QObject* parent = nullptr);
    virtual ~FilesystemModel() = default;

    virtual QVariant data(const QModelIndex& index, int role) const override;
};

#endif // FILESYSTEMSIDEBARMODEL_H
