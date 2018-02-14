// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef FILESYSTEMVIEW_H
#define FILESYSTEMVIEW_H

#include <QListView>

class QKeyEvent;
class FilesystemSidebarModel;

class FilesystemView : public QListView {
  Q_OBJECT

  public:
    explicit FilesystemView(QWidget* parent = nullptr);
    virtual ~FilesystemView() = default;

    virtual void setRootIndex(const QModelIndex& index) override;
    FilesystemSidebarModel* model() const;

    void setModel(FilesystemSidebarModel* model);

  public slots:
    void cdUp();

  protected:
    virtual void keyPressEvent(QKeyEvent* event) override;

  signals:
    void rootIndexChanged(const QModelIndex& index);

  private:
    FilesystemSidebarModel* m_model;
};

#endif // FILESYSTEMVIEW_H
