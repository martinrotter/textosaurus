// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef FILESYSTEMSIDEBAR_H
#define FILESYSTEMSIDEBAR_H

#include "src/gui/docks/dockwidget.h"

#include <QFileSystemModel>
#include <QListWidget>

class FileSystemSidebarModel : public QFileSystemModel {
  Q_OBJECT

  public:
    explicit FileSystemSidebarModel(QObject* parent = nullptr);
};

class FavoritesListWidget : public QListWidget {
  Q_OBJECT

  public:
    explicit FavoritesListWidget(QWidget* parent = nullptr);

  public slots:
    void loadFileItem(const QString& file_path);

  protected:
    void keyPressEvent(QKeyEvent* event);
};

class FilesystemSidebar : public DockWidget {
  Q_OBJECT

  public:
    explicit FilesystemSidebar(QWidget* parent = nullptr);

    Qt::DockWidgetArea initialArea() const;
    bool initiallyVisible() const;
    int initialWidth() const;

  public slots:
    void load();

  private slots:
    void addFileToFavorites();
    void openFavoriteFile(const QModelIndex& idx);
    void openFileFolder(const QModelIndex& idx);
    void goToParentFolder();

  signals:
    void openFileRequested(const QString& file_path);

  private:
    void saveFavorites() const;

  private:
    FileSystemSidebarModel* m_fsModel;
    QListView* m_fsView;
    FavoritesListWidget* m_lvFavorites;
};

#endif // FILESYSTEMSIDEBAR_H
