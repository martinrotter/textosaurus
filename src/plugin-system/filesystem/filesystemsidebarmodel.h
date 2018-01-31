// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef FILESYSTEMSIDEBARMODEL_H
#define FILESYSTEMSIDEBARMODEL_H

#include <QFileSystemModel>

class FilesystemSidebarModel : public QFileSystemModel {
  Q_OBJECT

  public:
    explicit FilesystemSidebarModel(QObject* parent = nullptr);
    virtual ~FilesystemSidebarModel() = default;

    virtual QVariant data(const QModelIndex& index, int role) const override;
};

#endif // FILESYSTEMSIDEBARMODEL_H
