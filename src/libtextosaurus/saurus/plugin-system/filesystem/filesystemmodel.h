// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef FILESYSTEMSIDEBARMODEL_H
#define FILESYSTEMSIDEBARMODEL_H

#include <QFileSystemModel>

class FilesystemPlugin;

class FilesystemModel : public QFileSystemModel {
  Q_OBJECT

  public:
    explicit FilesystemModel(FilesystemPlugin* plugin, QObject* parent = nullptr);
    virtual ~FilesystemModel() = default;

    virtual QVariant data(const QModelIndex& index, int role) const override;

  private:
    FilesystemPlugin* m_plugin;
};

#endif // FILESYSTEMSIDEBARMODEL_H
