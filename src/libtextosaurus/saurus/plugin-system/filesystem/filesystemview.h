// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

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

    QString normalizePath(const QString& path) const;
    QString currentFolder() const;
    QString selectedFileFolder() const;
    virtual void setRootIndex(const QModelIndex& index) override;

  public slots:
    void cdUp();
    void openFolder(const QModelIndex& idx);
    void openFolder(const QString& path);
    void highlightFile(const QString& file_path);

  protected:
    virtual void contextMenuEvent(QContextMenuEvent* event) override;
    virtual void keyPressEvent(QKeyEvent* event) override;

  signals:
    void rootIndexChanged(const QModelIndex& index);

  private:
    FilesystemModel* m_model;
};

#endif // FILESYSTEMVIEW_H
